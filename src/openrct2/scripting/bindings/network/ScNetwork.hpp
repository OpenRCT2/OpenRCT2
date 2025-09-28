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

    #include "../../ScriptEngine.h"
    #include "ScPlayer.hpp"
    #include "ScPlayerGroup.hpp"
    #include "ScSocket.hpp"

    #include <memory>

namespace OpenRCT2::Scripting
{
    class ScNetwork final : public ScBase
    {
        static JSValue mode_get(JSContext* ctx, JSValue thisVal);
        static JSValue numPlayers_get(JSContext* ctx, JSValue thisVal);
        static JSValue numGroups_get(JSContext* ctx, JSValue thisVal);
        static JSValue defaultGroup_get(JSContext* ctx, JSValue thisVal);
        static JSValue defaultGroup_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue groups_get(JSContext* ctx, JSValue thisVal);

        static JSValue players_get(JSContext* ctx, JSValue thisVal);

        static JSValue currentPlayer_get(JSContext* ctx, JSValue thisVal);

        static JSValue getPlayer(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue stats_get(JSContext* ctx, JSValue thisVal);

        static JSValue getGroup(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue addGroup(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue removeGroup(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue kickPlayer(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue sendMessage(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue createListener(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue createSocket(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

    public:
        JSValue New(JSContext* ctx);

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Network");
        }
    };

    extern ScNetwork gScNetwork;
} // namespace OpenRCT2::Scripting

#endif
