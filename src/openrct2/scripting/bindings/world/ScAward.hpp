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
    #include "../../../management/Award.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"

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

    class ScAward
    {
    private:
        size_t _index{};

    public:
        ScAward(size_t index);

        static void Register(duk_context* ctx);

    private:
        Award* GetAward() const;

        std::string type_get() const;
        std::string text_get() const;
        uint16_t monthsRemaining_get() const;
        bool positive_get() const;
        uint32_t imageId_get() const;
    };

} // namespace OpenRCT2::Scripting

#endif
