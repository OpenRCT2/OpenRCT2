/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../Context.h"
#    include "../../../config/Config.h"
#    include "../../../localisation/LocalisationService.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    enum class ScConfigurationKind
    {
        User,
        Shared,
        Park
    };

    class ScConfiguration
    {
    private:
        ScConfigurationKind _kind;
        DukValue _backingObject;

    public:
        // context.configuration
        ScConfiguration()
            : _kind(ScConfigurationKind::User)
        {
        }

        // context.sharedStorage / context.getParkStorage
        ScConfiguration(ScConfigurationKind kind, const DukValue& backingObject)
            : _kind(kind)
            , _backingObject(backingObject)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_method(ctx, &ScConfiguration::getAll, "getAll");
            dukglue_register_method(ctx, &ScConfiguration::get, "get");
            dukglue_register_method(ctx, &ScConfiguration::set, "set");
            dukglue_register_method(ctx, &ScConfiguration::has, "has");
        }

    private:
        std::pair<std::string_view, std::string_view> GetNextNamespace(std::string_view input) const
        {
            auto pos = input.find('.');
            if (pos == std::string_view::npos)
            {
                return std::make_pair(input, std::string_view());
            }

            return std::make_pair(input.substr(0, pos), input.substr(pos + 1));
        }

        std::pair<std::string_view, std::string_view> GetNamespaceAndKey(std::string_view input) const
        {
            auto pos = input.find_last_of('.');
            return pos == std::string_view::npos ? std::make_pair(std::string_view(), input)
                                                 : std::make_pair(input.substr(0, pos), input.substr(pos + 1));
        }

        std::optional<DukValue> GetNamespaceObject(std::string_view ns) const
        {
            auto store = _backingObject;
            if (!ns.empty())
            {
                auto k = ns;
                bool end;
                do
                {
                    auto [next, remainder] = GetNextNamespace(k);
                    store = store[next];
                    k = remainder;
                    end = store.type() == DukValue::Type::UNDEFINED || remainder.empty();
                } while (!end);
            }
            return store.type() == DukValue::OBJECT ? std::make_optional(store) : std::nullopt;
        }

        DukValue GetOrCreateNamespaceObject(duk_context* ctx, std::string_view ns) const
        {
            auto store = _backingObject;
            if (!ns.empty())
            {
                std::string_view k = ns;
                bool end;
                do
                {
                    auto [next, remainder] = GetNextNamespace(k);
                    auto subStore = store[next];
                    k = remainder;
                    if (subStore.type() == DukValue::Type::UNDEFINED)
                    {
                        store.push();
                        duk_push_object(ctx);
                        store = DukValue::copy_from_stack(ctx);
                        duk_put_prop_lstring(ctx, -2, next.data(), next.size());
                        duk_pop(ctx);
                    }
                    else
                    {
                        store = std::move(subStore);
                    }
                    end = remainder.empty();
                } while (!end);
            }
            return store;
        }

        bool IsValidNamespace(std::string_view ns) const
        {
            if (!ns.empty() && (ns[0] == '.' || ns[ns.size() - 1] == '.'))
            {
                return false;
            }

            if (_kind != ScConfigurationKind::Park)
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

        bool IsValidKey(std::string_view key) const
        {
            return !key.empty() && key.find('.') == std::string_view::npos;
        }

        DukValue getAll(const DukValue& dukNamespace) const
        {
            DukValue result;
            auto ctx = GetContext()->GetScriptEngine()->GetContext();

            std::string ns = "";
            if (dukNamespace.type() == DukValue::Type::STRING)
            {
                ns = dukNamespace.as_string();
            }
            else if (dukNamespace.type() != DukValue::Type::UNDEFINED)
            {
                duk_error(ctx, DUK_ERR_ERROR, "Namespace was invalid.");
            }

            if (IsValidNamespace(ns))
            {
                if (_kind == ScConfigurationKind::User)
                {
                    DukObject obj(ctx);
                    if (ns == "general")
                    {
                        obj.Set("general.language", gConfigGeneral.language);
                        obj.Set("general.showFps", gConfigGeneral.show_fps);
                    }
                    result = obj.Take();
                }
                else
                {
                    auto obj = GetNamespaceObject(ns);
                    result = obj ? *obj : DukObject(ctx).Take();
                }
            }
            else
            {
                duk_error(ctx, DUK_ERR_ERROR, "Namespace was invalid.");
            }
            return result;
        }

        DukValue get(const std::string& key, const DukValue& defaultValue) const
        {
            auto ctx = GetContext()->GetScriptEngine()->GetContext();
            if (_kind == ScConfigurationKind::User)
            {
                if (key == "general.language")
                {
                    auto* localisationService = GetContext()->GetLocalisationService();
                    auto language = localisationService->GetCurrentLanguage();
                    auto locale = "";
                    if (language >= 0 && static_cast<size_t>(language) < std::size(LanguagesDescriptors))
                    {
                        locale = LanguagesDescriptors[language].locale;
                    }
                    duk_push_string(ctx, locale);
                    return DukValue::take_from_stack(ctx);
                }
                if (key == "general.showFps")
                {
                    duk_push_boolean(ctx, gConfigGeneral.show_fps);
                    return DukValue::take_from_stack(ctx);
                }
            }
            else
            {
                auto [ns, n] = GetNamespaceAndKey(key);
                if (!IsValidNamespace(ns))
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Namespace was invalid.");
                }
                else if (!IsValidKey(n))
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Key was invalid.");
                }
                else
                {
                    auto obj = GetNamespaceObject(ns);
                    if (obj)
                    {
                        auto val = (*obj)[n];
                        if (val.type() != DukValue::Type::UNDEFINED)
                        {
                            return val;
                        }
                    }
                }
            }
            return defaultValue;
        }

        void set(const std::string& key, const DukValue& value) const
        {
            auto* scriptEngine = GetContext()->GetScriptEngine();
            auto ctx = scriptEngine->GetContext();
            if (_kind == ScConfigurationKind::User)
            {
                try
                {
                    if (key == "general.showFps")
                    {
                        gConfigGeneral.show_fps = value.as_bool();
                    }
                    else
                    {
                        duk_error(ctx, DUK_ERR_ERROR, "Property does not exist.");
                    }
                }
                catch (const DukException&)
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Invalid value for this property.");
                }
            }
            else
            {
                auto [ns, n] = GetNamespaceAndKey(key);
                if (!IsValidNamespace(ns))
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Namespace was invalid.");
                }
                else if (!IsValidKey(n))
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Key was invalid.");
                }
                else
                {
                    auto obj = GetOrCreateNamespaceObject(ctx, ns);
                    obj.push();
                    if (value.type() == DukValue::Type::UNDEFINED)
                    {
                        duk_del_prop_lstring(ctx, -1, n.data(), n.size());
                    }
                    else
                    {
                        value.push();
                        duk_put_prop_lstring(ctx, -2, n.data(), n.size());
                    }
                    duk_pop(ctx);

                    scriptEngine->SaveSharedStorage();
                }
            }
        }

        bool has(const std::string& key) const
        {
            auto val = get(key, DukValue());
            return val.type() != DukValue::Type::UNDEFINED;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
