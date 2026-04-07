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

    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScCompetition final : public ScBase
    {
        static JSValue status_get(JSContext* ctx, JSValue thisVal);
        static JSValue timeRemaining_get(JSContext* ctx, JSValue thisVal);
        static JSValue leaderboard_get(JSContext* ctx, JSValue thisVal);

        static JSValue start(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue stop(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue setPlayerScore(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue broadcastLeaderboard(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

    public:
        JSValue New(JSContext* ctx);
        void Register(JSContext* ctx);
    };

    extern ScCompetition gScCompetition;
} // namespace OpenRCT2::Scripting

#endif
