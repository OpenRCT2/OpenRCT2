/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScNetwork.hpp"

    #include "../../../Context.h"
    #include "../../../actions/NetworkModifyGroupAction.h"
    #include "../../../actions/PlayerKickAction.h"
    #include "../../../network/Network.h"
    #include "../../../network/NetworkAction.h"

namespace OpenRCT2::Scripting
{
    JSValue ScNetwork::mode_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        switch (NetworkGetMode())
        {
            case NETWORK_MODE_SERVER:
                return JSFromStdString(ctx, "server");
            case NETWORK_MODE_CLIENT:
                return JSFromStdString(ctx, "client");
        }
    #endif
        return JSFromStdString(ctx, "none");
    }

    JSValue ScNetwork::numPlayers_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        return JS_NewInt32(ctx, NetworkGetNumPlayers());
    #else
        return JS_NewInt32(ctx, 0);
    #endif
    }

    JSValue ScNetwork::numGroups_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        return JS_NewInt32(ctx, NetworkGetNumGroups());
    #else
        return JS_NewInt32(ctx, 0);
    #endif
    }

    JSValue ScNetwork::defaultGroup_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        return JS_NewInt32(ctx, NetworkGetDefaultGroup());
    #else
        return JS_NewInt32(ctx, 0);
    #endif
    }

    JSValue ScNetwork::defaultGroup_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
    #ifndef DISABLE_NETWORK
        JS_UNPACK_INT32(valueInt, ctx, value);

        auto action = NetworkModifyGroupAction(ModifyGroupType::SetDefault, valueInt);
        GameActions::Execute(&action);
    #endif
        return JS_UNDEFINED;
    }

    JSValue ScNetwork::groups_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue groups = JS_NewArray(ctx);
    #ifndef DISABLE_NETWORK
        auto numGroups = NetworkGetNumGroups();
        for (int32_t i = 0; i < numGroups; i++)
        {
            // TODO (mber)
            // auto groupId = NetworkGetGroupID(i);
            // groups.push_back(std::make_shared<ScPlayerGroup>(groupId));
        }
    #endif
        return groups;
    }

    JSValue ScNetwork::players_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue players = JS_NewArray(ctx);
    #ifndef DISABLE_NETWORK
        auto numPlayers = NetworkGetNumPlayers();
        for (int32_t i = 0; i < numPlayers; i++)
        {
            // TODO (mber)
            // auto playerId = NetworkGetPlayerID(i);
            // players.push_back(std::make_shared<ScPlayer>(playerId));
        }
    #endif
        return players;
    }

    JSValue ScNetwork::currentPlayer_get(JSContext* ctx, JSValue thisVal)
    {
        // TODO (mber)
        JS_ThrowInternalError(ctx, "not implemented");
        return JS_EXCEPTION;
        /*
        std::shared_ptr<ScPlayer> player;
    #ifndef DISABLE_NETWORK
        auto playerId = NetworkGetCurrentPlayerId();
        player = std::make_shared<ScPlayer>(playerId);
    #endif
        return player;
        */
    }

    JSValue ScNetwork::getPlayer(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        // TODO (mber)
        /*
    #ifndef DISABLE_NETWORK
        JS_UNPACK_INT32(id, ctx, argv[0]);
        if (GetTargetAPIVersion() < kApiVersionNetworkIDs)
        {
            auto index = id;
            auto numPlayers = NetworkGetNumPlayers();
            if (index < numPlayers)
            {
                auto playerId = NetworkGetPlayerID(index);
                return std::make_shared<ScPlayer>(playerId);
            }
        }
        else
        {
            auto index = NetworkGetPlayerIndex(id);
            if (index != -1)
            {
                return std::make_shared<ScPlayer>(id);
            }
        }

    #endif
    */
        return JS_NULL;
    }

    JSValue ScNetwork::stats_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        JSValue obj = JS_NewObject(ctx);
        auto networkStats = NetworkGetStats();
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
            auto numGroups = NetworkGetNumGroups();
            if (index < numGroups)
            {
                // TODO (mber)
                /*
                auto groupId = NetworkGetGroupID(index);
                return std::make_shared<ScPlayerGroup>(groupId);
                */
            }
        }
        else
        {
            auto index = NetworkGetGroupIndex(id);
            if (index != -1)
            {
                // TODO (mber)
                /*
                return std::make_shared<ScPlayerGroup>(id);
                */
            }
        }
    #endif
        return JS_NULL;
    }

    JSValue ScNetwork::addGroup(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
    #ifndef DISABLE_NETWORK
        auto networkModifyGroup = NetworkModifyGroupAction(ModifyGroupType::AddGroup);
        GameActions::Execute(&networkModifyGroup);
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
            auto numGroups = NetworkGetNumGroups();
            if (index < numGroups)
            {
                auto groupId = NetworkGetGroupID(index);
                auto networkAction = NetworkModifyGroupAction(ModifyGroupType::RemoveGroup, groupId);
                GameActions::Execute(&networkAction);
            }
        }
        else
        {
            auto index = NetworkGetGroupIndex(id);
            if (index != -1)
            {
                auto networkAction = NetworkModifyGroupAction(ModifyGroupType::RemoveGroup, id);
                GameActions::Execute(&networkAction);
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
            auto numPlayers = NetworkGetNumPlayers();
            if (index < numPlayers)
            {
                auto playerId = NetworkGetPlayerID(index);
                auto kickPlayerAction = PlayerKickAction(playerId);
                GameActions::Execute(&kickPlayerAction);
            }
        }
        else
        {
            auto index = NetworkGetPlayerIndex(id);
            if (index != -1)
            {
                auto kickPlayerAction = PlayerKickAction(id);
                GameActions::Execute(&kickPlayerAction);
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
            if (NetworkGetMode() == NETWORK_MODE_SERVER)
            {
                std::vector<uint8_t> playerIds;
                JSIterateArray(ctx, players, [&playerIds](JSContext* ctx, JSValue val) {
                    playerIds.push_back(static_cast<uint8_t>(JSToInt(ctx, val)));
                });
                if (!playerIds.empty())
                {
                    NetworkSendChat(message.c_str(), playerIds);
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
            NetworkSendChat(message.c_str());
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
    void ScNetwork::createListener()
    {
        JS_ThrowPlainError(ctx, "Networking has been disabled.");
        return JS_EXCEPTION;
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
    void ScNetwork::createSocket()
    {
        JS_ThrowPlainError(ctx, "Networking has been disabled.");
        return JS_EXCEPTION;
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
