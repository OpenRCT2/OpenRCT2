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

    #include "../../../profiling/Profiling.h"
    #include "../../Duktape.hpp"

namespace OpenRCT2::Scripting
{
    class ScProfiler
    {
    private:
        duk_context* _ctx{};

    public:
        ScProfiler(duk_context* ctx)
            : _ctx(ctx)
        {
        }

    private:
        DukValue getData()
        {
            const auto& data = OpenRCT2::Profiling::GetData();
            duk_push_array(_ctx);
            duk_uarridx_t index = 0;
            for (const auto& f : data)
            {
                DukObject obj(_ctx);
                obj.Set("name", f->GetName());
                obj.Set("callCount", f->GetCallCount());
                obj.Set("minTime", f->GetMinTime());
                obj.Set("maxTime", f->GetMaxTime());
                obj.Set("totalTime", f->GetTotalTime());
                obj.Set("parents", GetFunctionIndexArray(data, f->GetParents()));
                obj.Set("children", GetFunctionIndexArray(data, f->GetChildren()));
                obj.Take().push();
                duk_put_prop_index(_ctx, /* duk stack index */ -2, index);
                index++;
            }
            return DukValue::take_from_stack(_ctx);
        }

        DukValue GetFunctionIndexArray(
            const std::vector<OpenRCT2::Profiling::Function*>& all, const std::vector<OpenRCT2::Profiling::Function*>& items)
        {
            duk_push_array(_ctx);
            duk_uarridx_t index = 0;
            for (const auto& item : items)
            {
                auto it = std::find(all.begin(), all.end(), item);
                if (it != all.end())
                {
                    auto value = static_cast<duk_int_t>(std::distance(all.begin(), it));
                    duk_push_int(_ctx, value);
                    duk_put_prop_index(_ctx, /* duk stack index */ -2, index);
                    index++;
                }
            }
            return DukValue::take_from_stack(_ctx);
        }

        void start()
        {
            OpenRCT2::Profiling::Enable();
        }

        void stop()
        {
            OpenRCT2::Profiling::Disable();
        }

        void reset()
        {
            OpenRCT2::Profiling::ResetData();
        }

        bool enabled_get() const
        {
            return OpenRCT2::Profiling::IsEnabled();
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_method(ctx, &ScProfiler::getData, "getData");
            dukglue_register_method(ctx, &ScProfiler::start, "start");
            dukglue_register_method(ctx, &ScProfiler::stop, "stop");
            dukglue_register_method(ctx, &ScProfiler::reset, "reset");
            dukglue_register_property(ctx, &ScProfiler::enabled_get, nullptr, "enabled");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
