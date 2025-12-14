/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../Context.h"
    #include "../../../config/Config.h"
    #include "../../../localisation/LocalisationService.h"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScConfiguration;
    extern ScConfiguration gScConfiguration;

    enum class ScConfigurationKind
    {
        User,
        Shared,
        Park
    };

    class ScConfiguration final : public ScBase
    {
    private:
        struct ConfigurationData
        {
            ScConfigurationKind _kind;
            std::string _pluginName;
        };

        static JSValue GetParkStorageForPlugin(JSContext* ctx, const std::string& pluginName)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            JSValue parkStore = scriptEngine.GetParkStorage();
            JSValue pluginStore = JS_GetPropertyStr(ctx, parkStore, pluginName.c_str());

            // Create if it doesn't exist
            if (!JS_IsObject(pluginStore))
            {
                JS_FreeValue(ctx, pluginStore);
                pluginStore = JS_NewObject(ctx);
                JS_SetPropertyStr(ctx, parkStore, pluginName.c_str(), JS_DupValue(ctx, pluginStore));
            }

            return pluginStore;
        }

        static JSValue GetStoreForConfigType(JSContext* ctx, const ConfigurationData* data)
        {
            switch (data->_kind)
            {
                case ScConfigurationKind::Park:
                {
                    return GetParkStorageForPlugin(ctx, data->_pluginName);
                }
                case ScConfigurationKind::Shared:
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    return JS_DupValue(ctx, scriptEngine.GetSharedStorage());
                }
                case ScConfigurationKind::User:
                default:
                    Guard::Fail("Invalid ScConfigurationKind");
                    return JS_UNDEFINED;
            }
        }

        static std::pair<std::string_view, std::string_view> GetNextNamespace(std::string_view input)
        {
            auto pos = input.find('.');
            if (pos == std::string_view::npos)
            {
                return std::make_pair(input, std::string_view());
            }

            return std::make_pair(input.substr(0, pos), input.substr(pos + 1));
        }

        static std::pair<std::string_view, std::string_view> GetNamespaceAndKey(std::string_view input)
        {
            auto pos = input.find_last_of('.');
            return pos == std::string_view::npos ? std::make_pair(std::string_view(), input)
                                                 : std::make_pair(input.substr(0, pos), input.substr(pos + 1));
        }

        static JSValue GetNamespaceObject(JSContext* ctx, const ConfigurationData* data, std::string_view ns)
        {
            auto store = GetStoreForConfigType(ctx, data);
            if (!ns.empty())
            {
                auto k = ns;
                bool end;
                do
                {
                    auto [next, remainder] = GetNextNamespace(k);
                    auto oldStore = store;
                    store = JS_GetPropertyStr(ctx, store, std::string(next).c_str());
                    JS_FreeValue(ctx, oldStore);
                    k = remainder;
                    end = JS_IsUndefined(store) || remainder.empty();
                } while (!end);
            }
            return store;
        }

        static JSValue GetOrCreateNamespaceObject(JSContext* ctx, const ConfigurationData* data, std::string_view ns)
        {
            auto store = GetStoreForConfigType(ctx, data);
            if (!ns.empty())
            {
                std::string_view k = ns;
                bool end;
                do
                {
                    auto [next, remainder] = GetNextNamespace(k);
                    std::string nextStr(next);
                    k = remainder;

                    auto subStore = JS_GetPropertyStr(ctx, store, nextStr.c_str());
                    auto oldStore = store;
                    if (JS_IsUndefined(subStore))
                    {
                        store = JS_NewObject(ctx);
                        JS_SetPropertyStr(ctx, oldStore, nextStr.c_str(), JS_DupValue(ctx, store));
                        JS_FreeValue(ctx, oldStore);
                    }
                    else
                    {
                        store = subStore;
                        JS_FreeValue(ctx, oldStore);
                    }
                    end = remainder.empty();
                } while (!end);
            }
            return store;
        }

        static bool IsValidNamespace(std::string_view ns, ScConfigurationKind kind)
        {
            if (!ns.empty() && (ns[0] == '.' || ns[ns.size() - 1] == '.'))
            {
                return false;
            }

            if (kind != ScConfigurationKind::Park)
            {
                if (ns.empty())
                {
                    return false;
                }
                for (size_t i = 1; i < ns.size() - 1; i++)
                {
                    if (ns[i - 1] == '.' && ns[i] == '.')
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        static bool IsValidKey(std::string_view key)
        {
            return !key.empty() && key.find('.') == std::string_view::npos;
        }

        static JSValue getAll(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JSValue jsNamespace = argv[0];
            ConfigurationData* data = gScConfiguration.GetOpaque<ConfigurationData*>(thisVal);

            std::string ns = "";
            if (JS_IsString(jsNamespace))
            {
                ns = JSToStdString(ctx, jsNamespace);
            }
            else if (!JS_IsUndefined(jsNamespace))
            {
                JS_ThrowPlainError(ctx, "Namespace was invalid.");
                return JS_EXCEPTION;
            }

            if (IsValidNamespace(ns, data->_kind))
            {
                if (data->_kind == ScConfigurationKind::User)
                {
                    JSValue obj = JS_NewObject(ctx);
                    if (ns == "general")
                    {
                        auto& localisationService = GetContext()->GetLocalisationService();
                        auto locale = localisationService.GetCurrentLanguageLocale();
                        JS_SetPropertyStr(ctx, obj, "general.language", JSFromStdString(ctx, locale));
                        JS_SetPropertyStr(ctx, obj, "general.showFps", JS_NewBool(ctx, Config::Get().general.showFPS));
                    }
                    return obj;
                }
                else
                {
                    auto obj = GetNamespaceObject(ctx, data, ns);
                    if (!JS_IsObject(obj))
                    {
                        JS_FreeValue(ctx, obj);
                        return JS_NewObject(ctx);
                    }
                    return obj;
                }
            }
            else
            {
                JS_ThrowPlainError(ctx, "Namespace was invalid.");
                return JS_EXCEPTION;
            }
        }

        static JSValue get(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(key, ctx, argv[0]);
            JSValue defaultValue = argv[1];
            ConfigurationData* data = gScConfiguration.GetOpaque<ConfigurationData*>(thisVal);

            if (data->_kind == ScConfigurationKind::User)
            {
                if (key == "general.language")
                {
                    auto& localisationService = GetContext()->GetLocalisationService();
                    auto locale = localisationService.GetCurrentLanguageLocale();
                    return JSFromStdString(ctx, locale);
                }
                if (key == "general.showFps")
                {
                    return JS_NewBool(ctx, Config::Get().general.showFPS);
                }
            }
            else
            {
                auto [ns, n] = GetNamespaceAndKey(key);
                if (!IsValidNamespace(ns, data->_kind))
                {
                    JS_ThrowPlainError(ctx, "Namespace was invalid.");
                    return JS_EXCEPTION;
                }
                else if (!IsValidKey(n))
                {
                    JS_ThrowPlainError(ctx, "Key was invalid.");
                    return JS_EXCEPTION;
                }
                else
                {
                    auto obj = GetNamespaceObject(ctx, data, ns);
                    if (JS_IsObject(obj))
                    {
                        auto val = JS_GetPropertyStr(ctx, obj, std::string(n).c_str());
                        JS_FreeValue(ctx, obj);
                        if (!JS_IsUndefined(val))
                        {
                            return val;
                        }
                    }
                    else
                    {
                        JS_FreeValue(ctx, obj);
                    }
                }
            }
            return JS_DupValue(ctx, defaultValue);
        }

        static JSValue set(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(key, ctx, argv[0]);
            JSValue value = argv[1];
            ConfigurationData* data = gScConfiguration.GetOpaque<ConfigurationData*>(thisVal);

            if (data->_kind == ScConfigurationKind::User)
            {
                if (key == "general.showFps")
                {
                    if (JS_IsBool(value))
                    {
                        Config::Get().general.showFPS = JS_ToBool(ctx, value) > 0;
                        return JS_UNDEFINED;
                    }
                    else
                    {
                        JS_ThrowPlainError(ctx, "Invalid value for this property.");
                        return JS_EXCEPTION;
                    }
                }
                else
                {
                    JS_ThrowPlainError(ctx, "Property does not exist.");
                    return JS_EXCEPTION;
                }
            }
            else
            {
                auto [ns, n] = GetNamespaceAndKey(key);
                if (!IsValidNamespace(ns, data->_kind))
                {
                    JS_ThrowPlainError(ctx, "Namespace was invalid.");
                    return JS_EXCEPTION;
                }
                else if (!IsValidKey(n))
                {
                    JS_ThrowPlainError(ctx, "Key was invalid.");
                    return JS_EXCEPTION;
                }
                else
                {
                    auto obj = GetOrCreateNamespaceObject(ctx, data, ns);
                    JS_SetPropertyStr(ctx, obj, std::string(n).c_str(), JS_DupValue(ctx, value));
                    JS_FreeValue(ctx, obj);

                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.SaveSharedStorage();
                    return JS_UNDEFINED;
                }
            }
        }

        static JSValue has(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            std::array newArgs = { argv[0], JS_UNDEFINED };
            auto val = get(ctx, thisVal, 2, newArgs.data());
            if (JS_IsException(val))
            {
                return val;
            }
            return JS_NewBool(ctx, !JS_IsUndefined(val));
        }

        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CFUNC_DEF("getAll", 1, ScConfiguration::getAll),
            JS_CFUNC_DEF("get", 2, ScConfiguration::get),
            JS_CFUNC_DEF("set", 2, ScConfiguration::set),
            JS_CFUNC_DEF("has", 1, ScConfiguration::has),
        };

    public:
        // context.configuration
        JSValue New(JSContext* ctx)
        {
            return MakeWithOpaque(ctx, funcs, new ConfigurationData{ ScConfigurationKind::User, {} });
        }

        // context.sharedStorage / context.getParkStorage
        JSValue New(JSContext* ctx, ScConfigurationKind kind, std::string_view pluginName = {})
        {
            return MakeWithOpaque(ctx, funcs, new ConfigurationData{ kind, std::string(pluginName) });
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Configuration", Finalize);
        }

        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            ConfigurationData* data = gScConfiguration.GetOpaque<ConfigurationData*>(thisVal);
            if (data)
                delete data;
        }

    private:
    };
} // namespace OpenRCT2::Scripting

#endif
