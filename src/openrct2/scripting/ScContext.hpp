#pragma once

#include <dukglue/dukglue.h>
#include "HookEngine.h"
#include "ScDisposable.hpp"
#include "ScriptEngine.h"
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
            : _execInfo(execInfo),
              _hookEngine(hookEngine)
        {
        }

        void registerIntent(const DukValue& desc)
        {
        }

        std::shared_ptr<ScDisposable> subscribe(const std::string &hook, const DukValue &callback)
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
            return std::make_shared<ScDisposable>(
                [this, hookType, cookie]()
                {
                    _hookEngine.Unsubscribe(hookType, cookie);
                });
        }

        static void Register(duk_context * ctx)
        {
            dukglue_register_method(ctx, &ScContext::registerIntent, "registerIntent");
            dukglue_register_method(ctx, &ScContext::subscribe, "subscribe");
        }
    };
}
