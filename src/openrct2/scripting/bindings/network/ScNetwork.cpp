/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScNetwork.hpp"

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../actions/NetworkModifyGroupAction.h"
    #include "../../../actions/PlayerKickAction.h"
    #include "../../../network/Network.h"
    #include "../../../network/NetworkAction.h"

namespace OpenRCT2::Scripting
{
    JSValue ScNetwork::mode_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        switch (Network::GetMode())
        {
            case Network::Mode::server:
                return JSFromStdString(ctx, "server");
            case Network::Mode::client:
                return JSFromStdString(ctx, "client");
            default:
                break;
        }
    #endif
        return JSFromStdString(ctx, "none");
    }

    JSValue ScNetwork::numPlayers_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        return JS_NewInt32(ctx, Network::GetNumPlayers());
    #else
        return JS_NewInt32(ctx, 0);
    #endif
    }

    JSValue ScNetwork::numGroups_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        return JS_NewInt32(ctx, Network::GetNumGroups());
    #else
        return JS_NewInt32(ctx, 0);
    #endif
    }

    JSValue ScNetwork::defaultGroup_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        return JS_NewInt32(ctx, Network::GetDefaultGroup());
    #else
        return JS_NewInt32(ctx, 0);
    #endif
    }

    JSValue ScNetwork::defaultGroup_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
    #ifndef DISABLE_NETWORK
        JS_UNPACK_INT32(valueInt, ctx, value);

        auto action = GameActions::NetworkModifyGroupAction(GameActions::ModifyGroupType::SetDefault, valueInt);
        GameActions::Execute(&action, getGameState());
    #endif
        return JS_UNDEFINED;
    }

    JSValue ScNetwork::groups_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue groups = JS_NewArray(ctx);
    #ifndef DISABLE_NETWORK
        auto numGroups = Network::GetNumGroups();
        for (int32_t i = 0; i < numGroups; i++)
        {
            auto groupId = Network::GetGroupID(i);
            JS_SetPropertyInt64(ctx, groups, i, gScPlayerGroup.New(ctx, groupId));
        }
    #endif
        return groups;
    }

    JSValue ScNetwork::players_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue players = JS_NewArray(ctx);
    #ifndef DISABLE_NETWORK
        auto numPlayers = Network::GetNumPlayers();
        for (int32_t i = 0; i < numPlayers; i++)
        {
            auto playerId = Network::GetPlayerID(i);
            JS_SetPropertyInt64(ctx, players, i, gScPlayer.New(ctx, playerId));
        }
    #endif
        return players;
    }

    JSValue ScNetwork::currentPlayer_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        auto playerId = Network::GetCurrentPlayerId();
        return gScPlayer.New(ctx, playerId);
    #else
        return JS_NULL;
    #endif
    }

    JSValue ScNetwork::getPlayer(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
    #ifndef DISABLE_NETWORK
        JS_UNPACK_INT32(id, ctx, argv[0]);
        if (GetTargetAPIVersion() < kApiVersionNetworkIDs)
        {
            auto index = id;
            auto numPlayers = Network::GetNumPlayers();
            if (index < numPlayers)
            {
                auto playerId = Network::GetPlayerID(index);
                return gScPlayer.New(ctx, playerId);
            }
        }
        else
        {
            auto index = Network::GetPlayerIndex(id);
            if (index != -1)
            {
                return gScPlayer.New(ctx, id);
            }
        }
    #endif
        return JS_NULL;
    }

    JSValue ScNetwork::stats_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        JSValue obj = JS_NewObject(ctx);
        auto networkStats = Network::GetStats();
        {
            JSValue recvStatsArr = JS_NewArray(ctx);
            uint32_t index = 0;
            for (const auto v : networkStats.bytesReceived)
            {
                JS_SetPropertyUint32(ctx, recvStatsArr, index, JS_NewInt64(ctx, static_cast<int64_t>(v)));
                index++;
            }
            JS_SetPropertyStr(ctx, obj, "bytesReceived", recvStatsArr);
        }
        {
            JSValue sentStatsArr = JS_NewArray(ctx);
            uint32_t index = 0;
            for (auto v : networkStats.bytesSent)
            {
                JS_SetPropertyUint32(ctx, sentStatsArr, index, JS_NewInt64(ctx, static_cast<int64_t>(v)));
                index++;
            }
            JS_SetPropertyStr(ctx, obj, "bytesSent", sentStatsArr);
        }
        return obj;
    #else
        return JS_NULL;
    #endif
    }

    JSValue ScNetwork::getGroup(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
    #ifndef DISABLE_NETWORK
        JS_UNPACK_INT32(id, ctx, argv[0])

        if (GetTargetAPIVersion() < kApiVersionNetworkIDs)
        {
            auto index = id;
            auto numGroups = Network::GetNumGroups();
            if (index < numGroups)
            {
                auto groupId = Network::GetGroupID(index);
                return gScPlayerGroup.New(ctx, groupId);
            }
        }
        else
        {
            auto index = Network::GetGroupIndex(id);
            if (index != -1)
            {
                return gScPlayerGroup.New(ctx, id);
            }
        }
    #endif
        return JS_NULL;
    }

    JSValue ScNetwork::addGroup(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
    #ifndef DISABLE_NETWORK
        auto networkModifyGroup = GameActions::NetworkModifyGroupAction(GameActions::ModifyGroupType::AddGroup);
        GameActions::Execute(&networkModifyGroup, getGameState());
    #endif
        return JS_UNDEFINED;
    }

    JSValue ScNetwork::removeGroup(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
    #ifndef DISABLE_NETWORK
        JS_UNPACK_INT32(id, ctx, argv[0]);

        if (GetTargetAPIVersion() < kApiVersionNetworkIDs)
        {
            auto index = id;
            auto numGroups = Network::GetNumGroups();
            if (index < numGroups)
            {
                auto groupId = Network::GetGroupID(index);
                auto networkAction = GameActions::NetworkModifyGroupAction(GameActions::ModifyGroupType::RemoveGroup, groupId);
                GameActions::Execute(&networkAction, getGameState());
            }
        }
        else
        {
            auto index = Network::GetGroupIndex(id);
            if (index != -1)
            {
                auto networkAction = GameActions::NetworkModifyGroupAction(GameActions::ModifyGroupType::RemoveGroup, id);
                GameActions::Execute(&networkAction, getGameState());
            }
        }
    #endif
        return JS_UNDEFINED;
    }

    JSValue ScNetwork::kickPlayer(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
    #ifndef DISABLE_NETWORK
        JS_UNPACK_INT32(id, ctx, argv[0]);

        if (GetTargetAPIVersion() < kApiVersionNetworkIDs)
        {
            auto index = id;
            auto numPlayers = Network::GetNumPlayers();
            if (index < numPlayers)
            {
                auto playerId = Network::GetPlayerID(index);
                auto kickPlayerAction = GameActions::PlayerKickAction(playerId);
                GameActions::Execute(&kickPlayerAction, getGameState());
            }
        }
        else
        {
            auto index = Network::GetPlayerIndex(id);
            if (index != -1)
            {
                auto kickPlayerAction = GameActions::PlayerKickAction(id);
                GameActions::Execute(&kickPlayerAction, getGameState());
            }
        }
    #endif
        return JS_UNDEFINED;
    }

    JSValue ScNetwork::sendMessage(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
    #ifndef DISABLE_NETWORK
        JS_UNPACK_STR(message, ctx, argv[0]);
        JSValue players = argv[1];

        if (JS_IsArray(players))
        {
            if (Network::GetMode() == Network::Mode::server)
            {
                std::vector<uint8_t> playerIds;
                JSIterateArray(ctx, players, [&playerIds](JSContext* ctx2, JSValue val) {
                    playerIds.push_back(static_cast<uint8_t>(JSToInt(ctx2, val)));
                });
                if (!playerIds.empty())
                {
                    Network::SendChat(message.c_str(), playerIds);
                }
            }
            else
            {
                JS_ThrowPlainError(ctx, "Only servers can send private messages.");
                return JS_EXCEPTION;
            }
        }
        else
        {
            Network::SendChat(message.c_str());
        }
    #endif
        return JS_UNDEFINED;
    }

    #ifndef DISABLE_NETWORK
    JSValue ScNetwork::createListener(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        return JS_UNDEFINED;
        // TODO (mber)
        /*
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
        auto socket = std::make_shared<ScListener>(plugin);
        scriptEngine.AddSocket(socket);
        return socket;
        */
    }
    #else
    JSValue ScNetwork::createListener(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        return JS_ThrowPlainError(ctx, "Networking has been disabled.");
    }
    #endif

    #ifndef DISABLE_NETWORK
    JSValue ScNetwork::createSocket(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        return JS_UNDEFINED;
        // TODO (mber)
        /*
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
        auto socket = std::make_shared<ScSocket>(plugin);
        scriptEngine.AddSocket(socket);
        return socket;
        */
    }
    #else
    JSValue ScNetwork::createSocket(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        return JS_ThrowPlainError(ctx, "Networking has been disabled.");
    }
    #endif

    JSValue ScNetwork::New(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("mode", ScNetwork::mode_get, nullptr),
            JS_CGETSET_DEF("numGroups", ScNetwork::numGroups_get, nullptr),
            JS_CGETSET_DEF("numPlayers", ScNetwork::numPlayers_get, nullptr),
            JS_CGETSET_DEF("groups", ScNetwork::groups_get, nullptr),
            JS_CGETSET_DEF("players", ScNetwork::players_get, nullptr),
            JS_CGETSET_DEF("currentPlayer", ScNetwork::currentPlayer_get, nullptr),
            JS_CGETSET_DEF("defaultGroup", ScNetwork::defaultGroup_get, ScNetwork::defaultGroup_set),
            JS_CGETSET_DEF("stats", ScNetwork::stats_get, nullptr),

            JS_CFUNC_DEF("addGroup", 0, ScNetwork::addGroup),
            JS_CFUNC_DEF("getGroup", 1, ScNetwork::getGroup),
            JS_CFUNC_DEF("removeGroup", 1, ScNetwork::removeGroup),
            JS_CFUNC_DEF("getPlayer", 1, ScNetwork::getPlayer),
            JS_CFUNC_DEF("kickPlayer", 1, ScNetwork::kickPlayer),
            JS_CFUNC_DEF("sendMessage", 2, ScNetwork::sendMessage),

            JS_CFUNC_DEF("createListener", 0, ScNetwork::createListener),
            JS_CFUNC_DEF("createSocket", 0, ScNetwork::createSocket)
        };

        return MakeWithOpaque(ctx, funcs, nullptr);
    }

} // namespace OpenRCT2::Scripting

#endif
