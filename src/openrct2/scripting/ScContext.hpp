/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_SCRIPTING__

#    include "../actions/CustomAction.hpp"
#    include "../actions/ParkSetNameAction.hpp"
#    include "../actions/SmallSceneryPlaceAction.hpp"
#    include "../scenario/Scenario.h"
#    include "Duktape.hpp"
#    include "HookEngine.h"
#    include "ScConfiguration.hpp"
#    include "ScDisposable.hpp"
#    include "ScriptEngine.h"

#    include <cstdio>
#    include <memory>

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
        std::shared_ptr<ScConfiguration> configuration_get()
        {
            return std::make_shared<ScConfiguration>();
        }

        std::shared_ptr<ScConfiguration> sharedStorage_get()
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            return std::make_shared<ScConfiguration>(scriptEngine.GetSharedStorage());
        }

        void registerIntent(const DukValue& desc)
        {
        }

        int32_t getRandom(int32_t min, int32_t max)
        {
            ThrowIfGameStateNotMutable();
            if (min >= max)
                return min;
            int32_t range = max - min;
            return min + scenario_rand_max(range);
        }

        std::shared_ptr<ScDisposable> subscribe(const std::string& hook, const DukValue& callback)
        {
            auto hookType = GetHookType(hook);
            if (hookType == HOOK_TYPE::UNDEFINED)
            {
                throw DukException() << "Unknown hook type: " << hook;
            }

            if (!callback.is_function())
            {
                throw DukException() << "Expected function for callback";
            }

            auto owner = _execInfo.GetCurrentPlugin();
            if (owner == nullptr)
            {
                throw DukException() << "Not in a plugin context";
            }

            auto cookie = _hookEngine.Subscribe(hookType, owner, callback);
            return std::make_shared<ScDisposable>([this, hookType, cookie]() { _hookEngine.Unsubscribe(hookType, cookie); });
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
                auto action = CreateGameAction(actionid, args);
                if (action != nullptr)
                {
                    auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
                    if (isExecute)
                    {
                        action->SetCallback([this, plugin, callback](const GameAction*, const GameActionResult* res) -> void {
                            HandleGameActionResult(plugin, *res, callback);
                        });
                        GameActions::Execute(action.get());
                    }
                    else
                    {
                        auto res = GameActions::Query(action.get());
                        HandleGameActionResult(plugin, *res, callback);
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

        std::unique_ptr<GameAction> CreateGameAction(const std::string& actionid, const DukValue& args)
        {
            if (actionid == "parksetname")
            {
                auto name = args["name"].as_string();
                return std::make_unique<ParkSetNameAction>(name);
            }
            else if (actionid == "smallsceneryplace")
            {
                CoordsXYZD loc;
                loc.x = args["x"].as_int();
                loc.y = args["y"].as_int();
                loc.z = args["z"].as_int();
                loc.direction = args["direction"].as_int();
                uint8_t quadrant = args["quadrant"].as_int();
                uint8_t sceneryType = args["object"].as_int();
                uint8_t primaryColour = args["primaryColour"].as_int();
                uint8_t secondaryColour = args["secondaryColour"].as_int();
                return std::make_unique<SmallSceneryPlaceAction>(loc, quadrant, sceneryType, primaryColour, secondaryColour);
            }
            else
            {
                // Serialise args to json so that it can be sent
                auto ctx = args.context();
                if (args.type() == DukValue::Type::OBJECT)
                {
                    args.push();
                }
                else
                {
                    duk_push_object(ctx);
                }
                auto jsonz = duk_json_encode(ctx, -1);
                auto json = std::string(jsonz);
                duk_pop(ctx);
                return std::make_unique<CustomAction>(actionid, json);
            }
        }

        void HandleGameActionResult(
            const std::shared_ptr<Plugin>& plugin, const GameActionResult& res, const DukValue& callback)
        {
            // Construct result object
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto ctx = scriptEngine.GetContext();
            auto objIdx = duk_push_object(ctx);
            duk_push_int(ctx, static_cast<duk_int_t>(res.Error));
            duk_put_prop_string(ctx, objIdx, "error");

            if (res.Error != GA_ERROR::OK)
            {
                auto title = res.GetErrorTitle();
                duk_push_string(ctx, title.c_str());
                duk_put_prop_string(ctx, objIdx, "errorTitle");

                auto message = res.GetErrorMessage();
                duk_push_string(ctx, message.c_str());
                duk_put_prop_string(ctx, objIdx, "errorMessage");
            }

            duk_push_int(ctx, static_cast<duk_int_t>(res.Cost));
            duk_put_prop_string(ctx, objIdx, "cost");

            duk_push_int(ctx, static_cast<duk_int_t>(res.Expenditure));
            duk_put_prop_string(ctx, objIdx, "expenditureType");

            auto args = DukValue::take_from_stack(ctx);

            if (callback.is_function())
            {
                // Call the plugin callback and pass the result object
                scriptEngine.ExecutePluginCall(plugin, callback, { args }, false);
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

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScContext::configuration_get, nullptr, "configuration");
            dukglue_register_property(ctx, &ScContext::sharedStorage_get, nullptr, "sharedStorage");
            dukglue_register_method(ctx, &ScContext::getRandom, "getRandom");
            dukglue_register_method(ctx, &ScContext::registerIntent, "registerIntent");
            dukglue_register_method(ctx, &ScContext::subscribe, "subscribe");
            dukglue_register_method(ctx, &ScContext::queryAction, "queryAction");
            dukglue_register_method(ctx, &ScContext::executeAction, "executeAction");
            dukglue_register_method(ctx, &ScContext::registerAction, "registerAction");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
