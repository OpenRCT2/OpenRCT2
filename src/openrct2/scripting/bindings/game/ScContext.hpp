/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../../../OpenRCT2.h"
    #include "../../../actions/GameAction.h"
    #include "../../../interface/Screenshot.h"
    #include "../../../localisation/Formatting.h"
    #include "../../../object/ObjectManager.h"
    #include "../../../scenario/Scenario.h"
    #include "../../HookEngine.h"
    #include "../../IconNames.hpp"
    #include "../../ScriptEngine.h"
    #include "../game/ScConfiguration.hpp"
    #include "../game/ScDisposable.hpp"
    #include "../object/ScObjectManager.h"
    #include "../ride/ScTrackSegment.h"

    #include <cstdio>
    #include <memory>

namespace OpenRCT2::Scripting
{
    class ScContext;
    extern ScContext gScContext;

    class ScContext final : public ScBase
    {
    private:
        static JSValue apiVersion_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewInt32(ctx, kPluginApiVersion);
        }

        static JSValue configuration_get(JSContext* ctx, JSValue thisVal)
        {
            return gScConfiguration.New(ctx);
        }

        static JSValue sharedStorage_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_ThrowPlainError(ctx, "not implemented."); // this requires support in ScriptEngine.cpp
            /* TODO (mber)
            auto& scriptEngine = GetContext()->GetScriptEngine();
            return std::make_shared<ScConfiguration>(ScConfigurationKind::Shared, scriptEngine.GetSharedStorage());
            */
        }

        static JSValue GetParkStorageForPlugin(std::string_view pluginName)
        {
            return JS_NULL;
            /* TODO (mber)
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto parkStore = scriptEngine.GetParkStorage();
            auto pluginStore = parkStore[pluginName];

            // Create if it doesn't exist
            if (pluginStore.type() != DukValue::Type::OBJECT)
            {
                auto* ctx = scriptEngine.GetContext();
                parkStore.push();
                duk_push_object(ctx);
                duk_put_prop_lstring(ctx, -2, pluginName.data(), pluginName.size());
                duk_pop(ctx);

                pluginStore = parkStore[pluginName];
            }

            return std::make_shared<ScConfiguration>(ScConfigurationKind::Park, pluginStore);
            */
        }

        static JSValue getParkStorage(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            return JS_ThrowPlainError(ctx, "not implemented."); // this requires support in ScriptEngine.cpp
            /* TODO (mber) // dukPluginName is argv[0]
            auto& scriptEngine = GetContext()->GetScriptEngine();

            std::shared_ptr<ScConfiguration> result;
            if (dukPluginName.type() == DukValue::Type::STRING)
            {
                auto& pluginName = dukPluginName.as_string();
                if (pluginName.empty())
                {
                    duk_error(scriptEngine.GetContext(), DUK_ERR_ERROR, "Plugin name is empty");
                }
                result = GetParkStorageForPlugin(pluginName);
            }
            else if (dukPluginName.type() == DukValue::Type::UNDEFINED)
            {
                auto plugin = _execInfo.GetCurrentPlugin();
                if (plugin == nullptr)
                {
                    duk_error(
                        scriptEngine.GetContext(), DUK_ERR_ERROR, "Plugin name must be specified when used from console.");
                }
                result = GetParkStorageForPlugin(plugin->GetMetadata().Name);
            }
            else
            {
                duk_error(scriptEngine.GetContext(), DUK_ERR_ERROR, "Invalid plugin name.");
            }
            return result;
            */
        }

        static JSValue mode_get(JSContext* ctx, JSValue thisVal)
        {
            if (gLegacyScene == LegacyScene::titleSequence)
                return JSFromStdString(ctx, "title");
            else if (gLegacyScene == LegacyScene::scenarioEditor)
                return JSFromStdString(ctx, "scenario_editor");
            else if (gLegacyScene == LegacyScene::trackDesigner)
                return JSFromStdString(ctx, "track_designer");
            else if (gLegacyScene == LegacyScene::trackDesignsManager)
                return JSFromStdString(ctx, "track_manager");
            return JSFromStdString(ctx, "normal");
        }

        static JSValue paused_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, GameIsPaused());
        }

        static JSValue paused_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value)
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

            if (valueBool != GameIsPaused())
                PauseToggle();

            return JS_UNDEFINED;
        }

        static JSValue captureImage(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JSValue options = argv[0];
            try
            {
                auto rotation = JSToOptionalInt(ctx, options, "rotation");
                auto zoom = JSToOptionalInt(ctx, options, "zoom");
                if (!rotation.has_value() || !zoom.has_value())
                {
                    JS_ThrowPlainError(ctx, "Invalid options.");
                    return JS_EXCEPTION;
                }

                CaptureOptions captureOptions;
                captureOptions.Filename = fs::u8path(AsOrDefault(ctx, options, "filename", ""));
                captureOptions.Rotation = rotation.value() & 3;
                captureOptions.Zoom = ZoomLevel(zoom.value());
                captureOptions.Transparent = AsOrDefault(ctx, options, "transparent", false);

                JSValue jsPosition = JS_GetPropertyStr(ctx, options, "position");
                if (JS_IsObject(jsPosition))
                {
                    auto width = JSToOptionalInt(ctx, options, "width");
                    auto height = JSToOptionalInt(ctx, options, "height");
                    auto x = JSToOptionalInt(ctx, jsPosition, "x");
                    auto y = JSToOptionalInt(ctx, jsPosition, "y");

                    if (!width.has_value() || !height.has_value() || !x.has_value() || !y.has_value())
                    {
                        JS_FreeValue(ctx, jsPosition);
                        JS_ThrowPlainError(ctx, "Invalid options.");
                        return JS_EXCEPTION;
                    }

                    CaptureView view;
                    view.Width = width.value();
                    view.Height = height.value();
                    view.Position.x = x.value();
                    view.Position.y = y.value();
                    captureOptions.View = view;
                }
                JS_FreeValue(ctx, jsPosition);

                CaptureImage(captureOptions);
            }
            catch (const std::exception& ex)
            {
                JS_ThrowPlainError(ctx, "%s", ex.what());
                return JS_EXCEPTION;
            }

            return JS_UNDEFINED;
        }

        static JSValue getObject(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            // deprecated function, moved to ObjectManager.getObject.
            return gScObjectManager.getObject(ctx, thisVal, argc, argv);
        }

        static JSValue getAllObjects(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            // deprecated function, moved to ObjectManager.getAllObjects.
            return gScObjectManager.getAllObjects(ctx, thisVal, argc, argv);
        }

        static JSValue getTrackSegment(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(type, ctx, argv[0])

            if (type >= EnumValue(TrackElemType::count))
            {
                return JS_NULL;
            }
            else
            {
                return JS_NULL;
                /* TODO (mber)
                return GetObjectAsDukValue(ctx, std::make_shared<ScTrackSegment>(static_cast<TrackElemType>(type)));
                */
            }
        }

        static JSValue getAllTrackSegments(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            return JS_NULL;

            /* TODO (mber)
            std::vector<DukValue> result;
            for (uint16_t type = 0; type < EnumValue(TrackElemType::count); type++)
            {
                auto obj = std::make_shared<ScTrackSegment>(static_cast<TrackElemType>(type));
                if (obj != nullptr)
                {
                    result.push_back(GetObjectAsDukValue(ctx, obj));
                }
            }
            return result;
            */
        }

        static JSValue getRandom(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(min, ctx, argv[0]);
            JS_UNPACK_INT32(max, ctx, argv[1]);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

            if (min >= max)
                return JS_NewInt32(ctx, min);
            int32_t range = max - min;
            return JS_NewInt64(ctx, min + ScenarioRandMax(range));
        }

        static JSValue formatString(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            if (argc >= 1)
            {
                const JSValue jsFmt = argv[0];
                if (JS_IsString(jsFmt))
                {
                    FmtString fmt(JSToStdString(ctx, jsFmt));

                    std::vector<FormatArg_t> args;
                    for (int i = 1; i < argc; i++)
                    {
                        const JSValue jsArg = argv[i];
                        if (JS_IsNumber(jsArg))
                        {
                            args.emplace_back(JSToInt64(ctx, jsArg));
                        }
                        else if (JS_IsString(jsArg))
                        {
                            args.emplace_back(JSToStdString(ctx, jsArg));
                        }
                        else
                        {
                            JS_ThrowPlainError(ctx, "Invalid format argument.");
                            return JS_EXCEPTION;
                        }
                    }

                    auto result = FormatStringAny(fmt, args);
                    return JSFromStdString(ctx, result);
                }
                else
                {
                    JS_ThrowPlainError(ctx, "Invalid format string.");
                    return JS_EXCEPTION;
                }
            }
            JS_ThrowPlainError(ctx, "Invalid format string.");
            return JS_EXCEPTION;
        }

        static JSValue subscribe(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(hook, ctx, argv[0]);
            JSValue callback = argv[1];

            auto& scriptEngine = GetContext()->GetScriptEngine();

            auto hookType = GetHookType(hook);
            if (hookType == HookType::notDefined)
            {
                JS_ThrowPlainError(ctx, "Unknown hook type");
                return JS_EXCEPTION;
            }

            if (!JS_IsFunction(ctx, callback))
            {
                JS_ThrowPlainError(ctx, "Expected function for callback");
                return JS_EXCEPTION;
            }

            auto owner = scriptEngine.GetExecInfo().GetCurrentPlugin();
            if (owner == nullptr)
            {
                JS_ThrowPlainError(ctx, "Not in a plugin context");
                return JS_EXCEPTION;
            }

            auto& hookEngine = scriptEngine.GetHookEngine();
            if (!hookEngine.IsValidHookForPlugin(hookType, *owner))
            {
                JS_ThrowPlainError(ctx, "Hook type not available for this plugin type.");
                return JS_EXCEPTION;
            }

            auto cookie = hookEngine.Subscribe(hookType, owner, callback);
            return gScDisposable.New(ctx, [&hookEngine, hookType, cookie]() { hookEngine.Unsubscribe(hookType, cookie); });
        }

        static JSValue queryAction(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(action, ctx, argv[0]);
            JS_UNPACK_OBJECT(args, ctx, argv[1]);
            JS_UNPACK_CALLBACK(callback, ctx, argv[2]);

            throw std::runtime_error("not implemented");
            /* TODO (mber)
            QueryOrExecuteAction(action, args, callback, false);
            */
        }

        static JSValue executeAction(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(action, ctx, argv[0]);
            JS_UNPACK_OBJECT(args, ctx, argv[1]);
            JS_UNPACK_CALLBACK(callback, ctx, argv[2]);

            throw std::runtime_error("not implemented");
            /* TODO (mber)
            QueryOrExecuteAction(action, args, callback, true);
            */
        }

        /* TODO (mber)
        void QueryOrExecuteAction(const std::string& actionid, const DukValue& args, const DukValue& callback, bool isExecute)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto ctx = scriptEngine.GetContext();
            try
            {
                auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
                auto action = scriptEngine.CreateGameAction(actionid, args, plugin->GetMetadata().Name);
                if (action != nullptr)
                {
                    if (isExecute)
                    {
                        action->SetCallback(
                            [this, plugin,
                             callback](const GameActions::GameAction* act, const GameActions::Result* res) -> void {
                                HandleGameActionResult(plugin, *act, *res, callback);
                            });
                        GameActions::Execute(action.get(), getGameState());
                    }
                    else
                    {
                        auto res = GameActions::Query(action.get(), getGameState());
                        HandleGameActionResult(plugin, *action, res, callback);
                    }
                }
                else
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Unknown action.");
                }
            }
            catch (DukException&)
            {
                duk_error(ctx, DUK_ERR_ERROR, "Invalid action parameters.");
            }
        }

        void HandleGameActionResult(
            const std::shared_ptr<Plugin>& plugin, const GameActions::GameAction& action, const GameActions::Result& res,
            const DukValue& callback)
        {
            if (callback.is_function())
            {
                auto& scriptEngine = GetContext()->GetScriptEngine();
                auto dukResult = scriptEngine.GameActionResultToDuk(action, res);
                // Call the plugin callback and pass the result object
                scriptEngine.ExecutePluginCall(plugin, callback, { dukResult }, false);
            }
        }
        */

        static JSValue registerAction(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(action, ctx, argv[0]);
            JS_UNPACK_CALLBACK(query, ctx, argv[1]);
            JS_UNPACK_CALLBACK(execute, ctx, argv[2]);

            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            if (!scriptEngine.RegisterCustomAction(plugin, action, query, execute))
            {
                JS_ThrowPlainError(ctx, "action has already been registered.");
                return JS_EXCEPTION;
            }

            return JS_UNDEFINED;
        }

        static int32_t SetIntervalOrTimeout(const JSCallback& callback, int32_t delay, bool repeat)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();

            return scriptEngine.AddInterval(plugin, delay, repeat, callback);
        }

        static void ClearIntervalOrTimeout(int32_t handle)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            scriptEngine.RemoveInterval(plugin, handle);
        }

        static JSValue setInterval(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_CALLBACK(callback, ctx, argv[0]);
            JS_UNPACK_INT32(delay, ctx, argv[1]);
            return JS_NewInt32(ctx, SetIntervalOrTimeout(callback, delay, true));
        }

        static JSValue setTimeout(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_CALLBACK(callback, ctx, argv[0]);
            JS_UNPACK_INT32(delay, ctx, argv[1]);
            return JS_NewInt32(ctx, SetIntervalOrTimeout(callback, delay, false));
        }

        static JSValue clearInterval(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(handle, ctx, argv[0]);
            ClearIntervalOrTimeout(handle);
            return JS_UNDEFINED;
        }

        static JSValue clearTimeout(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(handle, ctx, argv[0]);
            ClearIntervalOrTimeout(handle);
            return JS_UNDEFINED;
        }

        static JSValue getIcon(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(iconName, ctx, argv[0]);
            return JS_NewInt64(ctx, GetIconByName(iconName));
        }

    public:
        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Context");
        }

        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("apiVersion", ScContext::apiVersion_get, nullptr),
                JS_CGETSET_DEF("configuration", ScContext::configuration_get, nullptr),
                JS_CGETSET_DEF("sharedStorage", ScContext::sharedStorage_get, nullptr),
                JS_CFUNC_DEF("getParkStorage", 1, ScContext::getParkStorage),
                JS_CGETSET_DEF("mode", ScContext::mode_get, nullptr),
                JS_CGETSET_DEF("paused", ScContext::paused_get, &ScContext::paused_set),
                JS_CFUNC_DEF("captureImage", 1, ScContext::captureImage),
                JS_CFUNC_DEF("getObject", 2, ScContext::getObject),
                JS_CFUNC_DEF("getAllObjects", 2, ScContext::getAllObjects),
                JS_CFUNC_DEF("getTrackSegment", 1, ScContext::getTrackSegment),
                JS_CFUNC_DEF("getAllTrackSegments", 0, ScContext::getAllTrackSegments),
                JS_CFUNC_DEF("getRandom", 2, ScContext::getRandom),
                JS_CFUNC_DEF("formatString", 0, ScContext::formatString),
                JS_CFUNC_DEF("subscribe", 2, ScContext::subscribe),
                JS_CFUNC_DEF("queryAction", 3, ScContext::queryAction),
                JS_CFUNC_DEF("executeAction", 3, ScContext::executeAction),
                JS_CFUNC_DEF("registerAction", 3, ScContext::registerAction),
                JS_CFUNC_DEF("setInterval", 2, ScContext::setInterval),
                JS_CFUNC_DEF("setTimeout", 2, ScContext::setTimeout),
                JS_CFUNC_DEF("clearInterval", 1, ScContext::clearInterval),
                JS_CFUNC_DEF("clearTimeout", 1, ScContext::clearTimeout),
                JS_CFUNC_DEF("getIcon", 1, ScContext::getIcon),
            };
            return MakeWithOpaque(ctx, funcs, nullptr);
        }
    };

    inline uint32_t ImageFromJSValue(JSContext* ctx, JSValue value)
    {
        uint32_t img{};
        if (JS_IsNumber(value))
        {
            JS_ToUint32(ctx, &img, value);
            if (GetTargetAPIVersion() <= kApiVersionG2Reorder)
            {
                img = NewIconIndex(img);
            }
        }
        else if (JS_IsString(value))
        {
            img = GetIconByName(JSToStdString(ctx, value));
        }
        return img;
    }
} // namespace OpenRCT2::Scripting

#endif
