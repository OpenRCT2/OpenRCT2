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

    #include "../../../profiling/Profiling.h"
    #include "../../ScriptEngine.h"
namespace OpenRCT2::Scripting
{
    class ScProfiler;
    extern ScProfiler gScProfiler;
    class ScProfiler final : public ScBase
    {
    private:
        static JSValue GetFunctionIndexArray(
            JSContext* ctx, const std::vector<Profiling::Function*>& all,
            const std::vector<Profiling::Function*>& items)
        {
            JSValue functionArray = JS_NewArray(ctx);
            int64_t index = 0;
            for (const auto& item : items)
            {
                auto it = std::find(all.begin(), all.end(), item);
                if (it != all.end())
                {
                    auto value = static_cast<uint64_t>(std::distance(all.begin(), it));
                    JS_SetPropertyInt64(ctx, functionArray, index++, JS_NewInt64(ctx, value));
                }
            }
            return functionArray;
        }

        static JSValue getData(JSContext* ctx, JSValue, int, JSValue*)
        {
            const auto& data = Profiling::GetData();
            JSValue profileData = JS_NewArray(ctx);
            int64_t index = 0;
            for (const auto& f : data)
            {
                JSValue val = JS_NewObject(ctx);
                JS_SetPropertyStr(ctx, val, "name", JSFromStdString(ctx, f->GetName()));
                JS_SetPropertyStr(ctx, val, "callCount", JS_NewInt64(ctx, f->GetCallCount()));
                JS_SetPropertyStr(ctx, val, "minTime", JS_NewFloat64(ctx, f->GetMinTime()));
                JS_SetPropertyStr(ctx, val, "maxTime", JS_NewFloat64(ctx, f->GetMaxTime()));
                JS_SetPropertyStr(ctx, val, "totalTime", JS_NewFloat64(ctx, f->getTotalTime()));
                JS_SetPropertyStr(ctx, val, "parents", GetFunctionIndexArray(ctx, data, f->GetParents()));
                JS_SetPropertyStr(ctx, val, "children", GetFunctionIndexArray(ctx, data, f->GetChildren()));
                JS_SetPropertyInt64(ctx, profileData, index++, val);
            }
            return profileData;
        }

        static JSValue start(JSContext*, JSValue, int, JSValue*)
        {
            Profiling::Enable();
            return JS_UNDEFINED;
        }

        static JSValue stop(JSContext*, JSValue, int, JSValue*)
        {
            Profiling::Disable();
            return JS_UNDEFINED;
        }

        static JSValue reset(JSContext*, JSValue, int, JSValue*)
        {
            Profiling::ResetData();
            return JS_UNDEFINED;
        }

        static JSValue enabled_get(JSContext* ctx, JSValue)
        {
            return JS_NewBool(ctx, Profiling::IsEnabled());
        }

    public:
        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CFUNC_DEF("getData", 0, ScProfiler::getData),
                JS_CFUNC_DEF("start", 0, ScProfiler::start),
                JS_CFUNC_DEF("stop", 0, ScProfiler::stop),
                JS_CFUNC_DEF("reset", 0, ScProfiler::reset),
                JS_CGETSET_DEF("enabled", ScProfiler::enabled_get, nullptr),
            };

            return MakeWithOpaque(ctx, funcs, nullptr);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Profiler");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
