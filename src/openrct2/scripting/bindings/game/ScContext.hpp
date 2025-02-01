/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../OpenRCT2.h"
    #include "../../../actions/GameAction.h"
    #include "../../../interface/Screenshot.h"
    #include "../../../localisation/Formatting.h"
    #include "../../../object/ObjectManager.h"
    #include "../../../scenario/Scenario.h"
    #include "../../Duktape.hpp"
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
    class ScContext
    {
    private:
        ScriptExecutionInfo& _execInfo;
        HookEngine& _hookEngine;

    public:
        ScContext(ScriptExecutionInfo& execInfo, HookEngine& hookEngine)
            : _execInfo(execInfo)
            , _hookEngine(hookEngine)
        {
        }

    private:
        int32_t apiVersion_get()
        {
            return kPluginApiVersion;
        }

        std::shared_ptr<ScConfiguration> configuration_get()
        {
            return std::make_shared<ScConfiguration>();
        }

        std::shared_ptr<ScConfiguration> sharedStorage_get()
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            return std::make_shared<ScConfiguration>(ScConfigurationKind::Shared, scriptEngine.GetSharedStorage());
        }

        std::shared_ptr<ScConfiguration> GetParkStorageForPlugin(std::string_view pluginName)
        {
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
        }

        std::shared_ptr<ScConfiguration> getParkStorage(const DukValue& dukPluginName)
        {
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
        }

        std::string mode_get()
        {
            if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
                return "title";
            else if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                return "scenario_editor";
            else if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
                return "track_designer";
            else if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
                return "track_manager";
            return "normal";
        }

        bool paused_get()
        {
            return GameIsPaused();
        }

        void paused_set(const bool& value)
        {
            ThrowIfGameStateNotMutable();
            if (value != GameIsPaused())
                PauseToggle();
        }

        void captureImage(const DukValue& options)
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            try
            {
                CaptureOptions captureOptions;
                captureOptions.Filename = fs::u8path(AsOrDefault(options["filename"], ""));
                captureOptions.Rotation = options["rotation"].as_uint() & 3;
                captureOptions.Zoom = ZoomLevel(options["zoom"].as_uint());
                captureOptions.Transparent = AsOrDefault(options["transparent"], false);

                auto dukPosition = options["position"];
                if (dukPosition.type() == DukValue::Type::OBJECT)
                {
                    CaptureView view;
                    view.Width = options["width"].as_int();
                    view.Height = options["height"].as_int();
                    view.Position.x = dukPosition["x"].as_int();
                    view.Position.y = dukPosition["y"].as_int();
                    captureOptions.View = view;
                }

                CaptureImage(captureOptions);
            }
            catch (const DukException&)
            {
                duk_error(ctx, DUK_ERR_ERROR, "Invalid options.");
            }
            catch (const std::exception& ex)
            {
                duk_error(ctx, DUK_ERR_ERROR, ex.what());
            }
        }

        DukValue getObject(const std::string& typez, int32_t index) const
        {
            // deprecated function, moved to ObjectManager.getObject.
            ScObjectManager objectManager;
            return objectManager.getObject(typez, index);
        }

        std::vector<DukValue> getAllObjects(const std::string& typez) const
        {
            // deprecated function, moved to ObjectManager.getAllObjects.
            ScObjectManager objectManager;
            return objectManager.getAllObjects(typez);
        }

        DukValue getTrackSegment(uint16_t type)
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            if (type >= EnumValue(TrackElemType::Count))
            {
                return ToDuk(ctx, nullptr);
            }
            else
            {
                return GetObjectAsDukValue(ctx, std::make_shared<ScTrackSegment>(static_cast<TrackElemType>(type)));
            }
        }

        std::vector<DukValue> getAllTrackSegments()
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();

            std::vector<DukValue> result;
            for (uint16_t type = 0; type < EnumValue(TrackElemType::Count); type++)
            {
                auto obj = std::make_shared<ScTrackSegment>(static_cast<TrackElemType>(type));
                if (obj != nullptr)
                {
                    result.push_back(GetObjectAsDukValue(ctx, obj));
                }
            }
            return result;
        }

        int32_t getRandom(int32_t min, int32_t max)
        {
            ThrowIfGameStateNotMutable();
            if (min >= max)
                return min;
            int32_t range = max - min;
            return min + ScenarioRandMax(range);
        }

        duk_ret_t formatString(duk_context* ctx)
        {
            auto nargs = duk_get_top(ctx);
            if (nargs >= 1)
            {
                auto dukFmt = DukValue::copy_from_stack(ctx, 0);
                if (dukFmt.type() == DukValue::Type::STRING)
                {
                    FmtString fmt(dukFmt.as_string());

                    std::vector<FormatArg_t> args;
                    for (duk_idx_t i = 1; i < nargs; i++)
                    {
                        auto dukArg = DukValue::copy_from_stack(ctx, i);
                        switch (dukArg.type())
                        {
                            case DukValue::Type::NUMBER:
                                args.push_back(dukArg.as_int());
                                break;
                            case DukValue::Type::STRING:
                                args.push_back(dukArg.as_string());
                                break;
                            default:
                                duk_error(ctx, DUK_ERR_ERROR, "Invalid format argument.");
                                break;
                        }
                    }

                    auto result = FormatStringAny(fmt, args);
                    duk_push_lstring(ctx, result.c_str(), result.size());
                }
                else
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Invalid format string.");
                }
            }
            else
            {
                duk_error(ctx, DUK_ERR_ERROR, "Invalid format string.");
            }
            return 1;
        }

    #ifdef _MSC_VER
        // HACK workaround to resolve issue #14853
        //      The exception thrown in duk_error was causing a crash when RAII kicked in for this lambda.
        //      Only ensuring it was not in the same generated method fixed it.
        __declspec(noinline)
    #endif
        std::shared_ptr<ScDisposable>
            CreateSubscription(HOOK_TYPE hookType, const DukValue& callback)
        {
            auto owner = _execInfo.GetCurrentPlugin();
            auto cookie = _hookEngine.Subscribe(hookType, owner, callback);
            return std::make_shared<ScDisposable>([this, hookType, cookie]() { _hookEngine.Unsubscribe(hookType, cookie); });
        }

        std::shared_ptr<ScDisposable> subscribe(const std::string& hook, const DukValue& callback)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto ctx = scriptEngine.GetContext();

            auto hookType = GetHookType(hook);
            if (hookType == HOOK_TYPE::UNDEFINED)
            {
                duk_error(ctx, DUK_ERR_ERROR, "Unknown hook type");
            }

            if (!callback.is_function())
            {
                duk_error(ctx, DUK_ERR_ERROR, "Expected function for callback");
            }

            auto owner = _execInfo.GetCurrentPlugin();
            if (owner == nullptr)
            {
                duk_error(ctx, DUK_ERR_ERROR, "Not in a plugin context");
            }

            if (!_hookEngine.IsValidHookForPlugin(hookType, *owner))
            {
                duk_error(ctx, DUK_ERR_ERROR, "Hook type not available for this plugin type.");
            }

            return CreateSubscription(hookType, callback);
        }

        void queryAction(const std::string& action, const DukValue& args, const DukValue& callback)
        {
            QueryOrExecuteAction(action, args, callback, false);
        }

        void executeAction(const std::string& action, const DukValue& args, const DukValue& callback)
        {
            QueryOrExecuteAction(action, args, callback, true);
        }

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
                            [this, plugin, callback](const GameAction* act, const GameActions::Result* res) -> void {
                                HandleGameActionResult(plugin, *act, *res, callback);
                            });
                        GameActions::Execute(action.get());
                    }
                    else
                    {
                        auto res = GameActions::Query(action.get());
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
            const std::shared_ptr<Plugin>& plugin, const GameAction& action, const GameActions::Result& res,
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

        void registerAction(const std::string& action, const DukValue& query, const DukValue& execute)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            auto ctx = scriptEngine.GetContext();
            if (!query.is_function())
            {
                duk_error(ctx, DUK_ERR_ERROR, "query was not a function.");
            }
            else if (!execute.is_function())
            {
                duk_error(ctx, DUK_ERR_ERROR, "execute was not a function.");
            }
            else if (!scriptEngine.RegisterCustomAction(plugin, action, query, execute))
            {
                duk_error(ctx, DUK_ERR_ERROR, "action has already been registered.");
            }
        }

        int32_t SetIntervalOrTimeout(DukValue callback, int32_t delay, bool repeat)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto ctx = scriptEngine.GetContext();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();

            int32_t handle = 0;
            if (callback.is_function())
            {
                handle = scriptEngine.AddInterval(plugin, delay, repeat, std::move(callback));
            }
            else
            {
                duk_error(ctx, DUK_ERR_ERROR, "callback was not a function.");
            }
            return handle;
        }

        void ClearIntervalOrTimeout(int32_t handle)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            scriptEngine.RemoveInterval(plugin, handle);
        }

        int32_t setInterval(DukValue callback, int32_t delay)
        {
            return SetIntervalOrTimeout(callback, delay, true);
        }

        int32_t setTimeout(DukValue callback, int32_t delay)
        {
            return SetIntervalOrTimeout(callback, delay, false);
        }

        void clearInterval(int32_t handle)
        {
            ClearIntervalOrTimeout(handle);
        }

        void clearTimeout(int32_t handle)
        {
            ClearIntervalOrTimeout(handle);
        }

        int32_t getIcon(const std::string& iconName)
        {
            return GetIconByName(iconName);
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScContext::apiVersion_get, nullptr, "apiVersion");
            dukglue_register_property(ctx, &ScContext::configuration_get, nullptr, "configuration");
            dukglue_register_property(ctx, &ScContext::sharedStorage_get, nullptr, "sharedStorage");
            dukglue_register_method(ctx, &ScContext::getParkStorage, "getParkStorage");
            dukglue_register_property(ctx, &ScContext::mode_get, nullptr, "mode");
            dukglue_register_property(ctx, &ScContext::paused_get, &ScContext::paused_set, "paused");
            dukglue_register_method(ctx, &ScContext::captureImage, "captureImage");
            dukglue_register_method(ctx, &ScContext::getObject, "getObject");
            dukglue_register_method(ctx, &ScContext::getAllObjects, "getAllObjects");
            dukglue_register_method(ctx, &ScContext::getTrackSegment, "getTrackSegment");
            dukglue_register_method(ctx, &ScContext::getAllTrackSegments, "getAllTrackSegments");
            dukglue_register_method(ctx, &ScContext::getRandom, "getRandom");
            dukglue_register_method_varargs(ctx, &ScContext::formatString, "formatString");
            dukglue_register_method(ctx, &ScContext::subscribe, "subscribe");
            dukglue_register_method(ctx, &ScContext::queryAction, "queryAction");
            dukglue_register_method(ctx, &ScContext::executeAction, "executeAction");
            dukglue_register_method(ctx, &ScContext::registerAction, "registerAction");
            dukglue_register_method(ctx, &ScContext::setInterval, "setInterval");
            dukglue_register_method(ctx, &ScContext::setTimeout, "setTimeout");
            dukglue_register_method(ctx, &ScContext::clearInterval, "clearInterval");
            dukglue_register_method(ctx, &ScContext::clearTimeout, "clearTimeout");
            dukglue_register_method(ctx, &ScContext::getIcon, "getIcon");
        }
    };

    uint32_t ImageFromDuk(const DukValue& d)
    {
        uint32_t img{};
        if (d.type() == DukValue::Type::NUMBER)
        {
            img = d.as_uint();
            if (GetTargetAPIVersion() <= kApiVersionG2Reorder)
            {
                img = NewIconIndex(d.as_uint());
            }
        }
        else if (d.type() == DukValue::Type::STRING)
        {
            img = GetIconByName(d.as_c_string());
        }
        return img;
    }
} // namespace OpenRCT2::Scripting

#endif
