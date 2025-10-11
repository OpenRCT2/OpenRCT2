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

    #include "../../../Context.h"
    #include "../../../management/Award.h"
    #include "../../ScriptEngine.h"

    #include <quickjs.h>
    #include <string>

namespace OpenRCT2::Scripting
{
    static constexpr const char* AwardTypes[] = {
        "mostUntidy",
        "mostTidy",
        "bestRollerCoasters",
        "bestValue",
        "mostBeautiful",
        "worstValue",
        "safest",
        "bestStaff",
        "bestFood",
        "worstFood",
        "bestToilets",
        "mostDisappointing",
        "bestWaterRides",
        "bestCustomDesignedRides",
        "mostDazzlingRideColours",
        "mostConfusingLayout",
        "bestGentleRides",
    };

    inline std::optional<std::string> AwardTypeToString(AwardType awardType)
    {
        auto index = static_cast<size_t>(awardType);
        if (index < std::size(AwardTypes))
        {
            return AwardTypes[index];
        }
        return std::nullopt;
    }

    inline std::optional<AwardType> StringToAwardType(std::string_view awardType)
    {
        auto it = std::find(std::begin(AwardTypes), std::end(AwardTypes), awardType);
        if (it != std::end(AwardTypes))
        {
            return std::optional(static_cast<AwardType>(std::distance(std::begin(AwardTypes), it)));
        }
        return std::nullopt;
    }

    class ScAward;
    extern ScAward gScAward;

    class ScAward final : public ScBase
    {
    public:
        JSValue New(JSContext* ctx, size_t index);
        void Register(JSContext* ctx);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
        static Award* GetAward(JSValue thisVal);

        static JSValue type_get(JSContext* ctx, JSValue thisVal);
        static JSValue text_get(JSContext* ctx, JSValue thisVal);
        static JSValue monthsRemaining_get(JSContext* ctx, JSValue thisVal);
        static JSValue positive_get(JSContext* ctx, JSValue thisVal);
        static JSValue imageId_get(JSContext* ctx, JSValue thisVal);

        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("type", &ScAward::type_get, nullptr), JS_CGETSET_DEF("text", &ScAward::text_get, nullptr),
            JS_CGETSET_DEF("positive", &ScAward::positive_get, nullptr),
            JS_CGETSET_DEF("imageId", &ScAward::imageId_get, nullptr),
            JS_CGETSET_DEF("monthsRemaining", &ScAward::monthsRemaining_get, nullptr)
        };
    };

} // namespace OpenRCT2::Scripting

#endif
