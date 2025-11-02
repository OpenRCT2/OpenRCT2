/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScPlayer.hpp"

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../actions/PlayerSetGroupAction.h"
    #include "../../../network/Network.h"
    #include "../../../network/NetworkAction.h"

namespace OpenRCT2::Scripting
{
    using OpaquePlayerData = struct
    {
        int32_t id;
    };

    int32_t ScPlayer::GetPlayerId(JSValue thisVal)
    {
        OpaquePlayerData* data = gScPlayer.GetOpaque<OpaquePlayerData*>(thisVal);
        if (data)
            return data->id;
        return -1;
    }

    JSValue ScPlayer::id_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt32(ctx, GetPlayerId(thisVal));
    }

    JSValue ScPlayer::name_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetPlayerIndex(GetPlayerId(thisVal));
        if (index == -1)
            return JSFromStdString(ctx, {});
        return JSFromStdString(ctx, Network::GetPlayerName(index));
    #else
        return JSFromStdString(ctx, {});
    #endif
    }

    JSValue ScPlayer::group_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetPlayerIndex(GetPlayerId(thisVal));
        if (index == -1)
            return JS_NewInt32(ctx, {});
        return JS_NewInt32(ctx, Network::GetPlayerGroup(index));
    #else
        return JS_NewInt32(ctx, 0);
    #endif
    }
    JSValue ScPlayer::group_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
    #ifndef DISABLE_NETWORK
        JS_UNPACK_INT32(valueInt, ctx, value)
        auto playerSetGroupAction = GameActions::PlayerSetGroupAction(GetPlayerId(thisVal), valueInt);
        GameActions::Execute(&playerSetGroupAction, getGameState());
    #endif
        return JS_UNDEFINED;
    }

    JSValue ScPlayer::ping_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetPlayerIndex(GetPlayerId(thisVal));
        if (index == -1)
            return JS_NewInt32(ctx, {});
        return JS_NewInt32(ctx, Network::GetPlayerPing(index));
    #else
        return JS_NewInt32(ctx, 0);
    #endif
    }

    JSValue ScPlayer::commandsRan_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetPlayerIndex(GetPlayerId(thisVal));
        if (index == -1)
            return JS_NewInt32(ctx, {});
        return JS_NewInt32(ctx, Network::GetPlayerCommandsRan(index));
    #else
        return JS_NewInt32(ctx, 0);
    #endif
    }

    JSValue ScPlayer::moneySpent_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetPlayerIndex(GetPlayerId(thisVal));
        if (index == -1)
            return JS_NewInt64(ctx, {});
        return JS_NewInt64(ctx, Network::GetPlayerMoneySpent(index));
    #else
        return JS_NewInt64(ctx, 0);
    #endif
    }

    JSValue ScPlayer::ipAddress_get(JSContext* ctx, JSValue thisVal)
    {
        return JSFromStdString(ctx, Network::GetPlayerIPAddress(GetPlayerId(thisVal)));
    }

    JSValue ScPlayer::publicKeyHash_get(JSContext* ctx, JSValue thisVal)
    {
        return JSFromStdString(ctx, Network::GetPlayerPublicKeyHash(GetPlayerId(thisVal)));
    }

    JSValue ScPlayer::New(JSContext* ctx, int32_t id)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("id", ScPlayer::id_get, nullptr),
            JS_CGETSET_DEF("name", ScPlayer::name_get, nullptr),
            JS_CGETSET_DEF("group", ScPlayer::group_get, ScPlayer::group_set),
            JS_CGETSET_DEF("ping", ScPlayer::ping_get, nullptr),
            JS_CGETSET_DEF("commandsRan", ScPlayer::commandsRan_get, nullptr),
            JS_CGETSET_DEF("moneySpent", ScPlayer::moneySpent_get, nullptr),
            JS_CGETSET_DEF("ipAddress", ScPlayer::ipAddress_get, nullptr),
            JS_CGETSET_DEF("publicKeyHash", ScPlayer::publicKeyHash_get, nullptr),
        };
        return MakeWithOpaque(ctx, funcs, new OpaquePlayerData{ id });
    }

    void ScPlayer::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "Player", Finalize);
    }

    void ScPlayer::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        OpaquePlayerData* data = gScPlayer.GetOpaque<OpaquePlayerData*>(thisVal);
        if (data)
            delete data;
    }

} // namespace OpenRCT2::Scripting

#endif
