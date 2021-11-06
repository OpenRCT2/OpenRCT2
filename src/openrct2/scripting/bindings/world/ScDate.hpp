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
#    include "../../../Date.h"
#    include "../../../Game.h"
#    include "../../../GameState.h"
#    include "../../../common.h"
#    include "../../../localisation/Date.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScDate
    {
    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScDate::monthsElapsed_get, &ScDate::monthsElapsed_set, "monthsElapsed");
            dukglue_register_property(ctx, &ScDate::monthProgress_get, &ScDate::monthProgress_set, "monthProgress");
            dukglue_register_property(ctx, &ScDate::yearsElapsed_get, nullptr, "yearsElapsed");
            dukglue_register_property(ctx, &ScDate::ticksElapsed_get, nullptr, "ticksElapsed");
            dukglue_register_property(ctx, &ScDate::day_get, nullptr, "day");
            dukglue_register_property(ctx, &ScDate::month_get, nullptr, "month");
            dukglue_register_property(ctx, &ScDate::year_get, nullptr, "year");
        }

    private:
        int32_t monthsElapsed_get() const
        {
            const auto& date = GetDate();
            return date.GetMonthsElapsed();
        }

        void monthsElapsed_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            gDateMonthsElapsed = static_cast<int32_t>(value);
        }

        uint32_t monthProgress_get() const
        {
            const auto& date = GetDate();
            return date.GetMonthTicks();
        }

        void monthProgress_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            gDateMonthTicks = value;
        }

        uint32_t yearsElapsed_get() const
        {
            const auto& date = GetDate();
            return date.GetMonthsElapsed() / 8;
        }

        uint32_t ticksElapsed_get() const
        {
            return gCurrentTicks;
        }

        int32_t day_get() const
        {
            const auto& date = GetDate();
            return date.GetDay() + 1;
        }

        int32_t month_get() const
        {
            const auto& date = GetDate();
            return date.GetMonth();
        }

        int32_t year_get() const
        {
            const auto& date = GetDate();
            return date.GetYear() + 1;
        }

    private:
        const Date& GetDate() const
        {
            auto gameState = GetContext()->GetGameState();
            return gameState->GetDate();
        }
    };
} // namespace OpenRCT2::Scripting

#endif
