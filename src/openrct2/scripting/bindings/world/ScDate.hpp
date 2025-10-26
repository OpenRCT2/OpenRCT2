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
    #include "../../../Date.h"
    #include "../../../Game.h"
    #include "../../../GameState.h"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScDate;
    extern ScDate gScDate;
    class ScDate final : public ScBase
    {
        static JSValue monthsElapsed_get(JSContext* ctx, JSValue)
        {
            const auto& date = GetDate();
            return JS_NewInt32(ctx, date.GetMonthsElapsed());
        }

        static JSValue monthsElapsed_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            OpenRCT2::getGameState().date = Date{ value, GetDate().GetMonthTicks() };
            return JS_UNDEFINED;
        }

        static JSValue monthProgress_get(JSContext* ctx, JSValue)
        {
            const auto& date = GetDate();
            return JS_NewUint32(ctx, date.GetMonthTicks());
        }

        static JSValue monthProgress_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_INT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            OpenRCT2::getGameState().date = Date{ GetDate().GetMonthsElapsed(), static_cast<uint16_t>(value) };
            return JS_UNDEFINED;
        }

        static JSValue yearsElapsed_get(JSContext* ctx, JSValue)
        {
            const auto& date = GetDate();
            return JS_NewUint32(ctx, date.GetYear());
        }

        static JSValue ticksElapsed_get(JSContext* ctx, JSValue)
        {
            return JS_NewUint32(ctx, getGameState().currentTicks);
        }

        static JSValue day_get(JSContext* ctx, JSValue)
        {
            return JS_NewInt32(ctx, getGameState().date.GetDay() + 1);
        }

        static JSValue month_get(JSContext* ctx, JSValue)
        {
            return JS_NewInt32(ctx, getGameState().date.GetMonth());
        }

        static JSValue year_get(JSContext* ctx, JSValue)
        {
            return JS_NewInt32(ctx, getGameState().date.GetYear() + 1);
        }

    public:
        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("monthsElapsed", ScDate::monthsElapsed_get, ScDate::monthsElapsed_set),
                JS_CGETSET_DEF("monthProgress", ScDate::monthProgress_get, ScDate::monthProgress_set),
                JS_CGETSET_DEF("yearsElapsed", ScDate::yearsElapsed_get, nullptr),
                JS_CGETSET_DEF("ticksElapsed", ScDate::ticksElapsed_get, nullptr),
                JS_CGETSET_DEF("day", ScDate::day_get, nullptr),
                JS_CGETSET_DEF("month", ScDate::month_get, nullptr),
                JS_CGETSET_DEF("year", ScDate::year_get, nullptr),
            };

            return MakeWithOpaque(ctx, funcs, nullptr);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Date");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
