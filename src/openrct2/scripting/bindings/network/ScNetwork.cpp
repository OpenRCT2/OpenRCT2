/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScNetwork.hpp"

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../actions/NetworkModifyGroupAction.h"
    #include "../../../actions/PlayerKickAction.h"
    #include "../../../network/Network.h"
    #include "../../../network/NetworkAction.h"

namespace OpenRCT2::Scripting
{
    ScNetwork::ScNetwork(duk_context* ctx)
        : _context(ctx)
    {
    }

    std::string ScNetwork::mode_get() const
    {
    #ifndef DISABLE_NETWORK
        switch (Network::GetMode())
        {
            case Network::Mode::server:
                return "server";
            case Network::Mode::client:
                return "client";
            default:
                break;
        }
    #endif
        return "none";
    }

    int32_t ScNetwork::numPlayers_get() const
    {
    #ifndef DISABLE_NETWORK
        return Network::GetNumPlayers();
    #else
        return 0;
    #endif
    }

    int32_t ScNetwork::numGroups_get() const
    {
    #ifndef DISABLE_NETWORK
        return Network::GetNumGroups();
    #else
        return 0;
    #endif
    }

    int32_t ScNetwork::defaultGroup_get() const
    {
    #ifndef DISABLE_NETWORK
        return Network::GetDefaultGroup();
    #else
        return 0;
    #endif
    }

    void ScNetwork::defaultGroup_set(int32_t value)
    {
    #ifndef DISABLE_NETWORK
        auto action = GameActions::NetworkModifyGroupAction(GameActions::ModifyGroupType::SetDefault, value);
        GameActions::Execute(&action, getGameState());
    #endif
    }

    std::vector<std::shared_ptr<ScPlayerGroup>> ScNetwork::groups_get() const
    {
        std::vector<std::shared_ptr<ScPlayerGroup>> groups;
    #ifndef DISABLE_NETWORK
        auto numGroups = Network::GetNumGroups();
        for (int32_t i = 0; i < numGroups; i++)
        {
            auto groupId = Network::GetGroupID(i);
            groups.push_back(std::make_shared<ScPlayerGroup>(groupId));
        }
    #endif
        return groups;
    }

    std::vector<std::shared_ptr<ScPlayer>> ScNetwork::players_get() const
    {
        std::vector<std::shared_ptr<ScPlayer>> players;
    #ifndef DISABLE_NETWORK
        auto numPlayers = Network::GetNumPlayers();
        for (int32_t i = 0; i < numPlayers; i++)
        {
            auto playerId = Network::GetPlayerID(i);
            players.push_back(std::make_shared<ScPlayer>(playerId));
        }
    #endif
        return players;
    }

    std::shared_ptr<ScPlayer> ScNetwork::currentPlayer_get() const
    {
        std::shared_ptr<ScPlayer> player;
    #ifndef DISABLE_NETWORK
        auto playerId = Network::GetCurrentPlayerId();
        player = std::make_shared<ScPlayer>(playerId);
    #endif
        return player;
    }

    std::shared_ptr<ScPlayer> ScNetwork::getPlayer(int32_t id) const
    {
    #ifndef DISABLE_NETWORK
        if (GetTargetAPIVersion() < kApiVersionNetworkIDs)
        {
            auto index = id;
            auto numPlayers = Network::GetNumPlayers();
            if (index < numPlayers)
            {
                auto playerId = Network::GetPlayerID(index);
                return std::make_shared<ScPlayer>(playerId);
            }
        }
        else
        {
            auto index = Network::GetPlayerIndex(id);
            if (index != -1)
            {
                return std::make_shared<ScPlayer>(id);
            }
        }

    #endif
        return nullptr;
    }

    DukValue ScNetwork::stats_get() const
    {
    #ifndef DISABLE_NETWORK
        auto obj = DukObject(_context);
        auto networkStats = Network::GetStats();
        {
            duk_push_array(_context);
            duk_uarridx_t index = 0;
            for (auto v : networkStats.bytesReceived)
            {
                duk_push_number(_context, v);
                duk_put_prop_index(_context, -2, index);
                index++;
            }
            obj.Set("bytesReceived", DukValue::take_from_stack(_context));
        }
        {
            duk_push_array(_context);
            duk_uarridx_t index = 0;
            for (auto v : networkStats.bytesSent)
            {
                duk_push_number(_context, v);
                duk_put_prop_index(_context, -2, index);
                index++;
            }
            obj.Set("bytesSent", DukValue::take_from_stack(_context));
        }
        return obj.Take();
    #else
        return ToDuk(_context, nullptr);
    #endif
    }

    std::shared_ptr<ScPlayerGroup> ScNetwork::getGroup(int32_t id) const
    {
    #ifndef DISABLE_NETWORK
        if (GetTargetAPIVersion() < kApiVersionNetworkIDs)
        {
            auto index = id;
            auto numGroups = Network::GetNumGroups();
            if (index < numGroups)
            {
                auto groupId = Network::GetGroupID(index);
                return std::make_shared<ScPlayerGroup>(groupId);
            }
        }
        else
        {
            auto index = Network::GetGroupIndex(id);
            if (index != -1)
            {
                return std::make_shared<ScPlayerGroup>(id);
            }
        }
    #endif
        return nullptr;
    }

    void ScNetwork::addGroup()
    {
    #ifndef DISABLE_NETWORK
        auto networkModifyGroup = GameActions::NetworkModifyGroupAction(GameActions::ModifyGroupType::AddGroup);
        GameActions::Execute(&networkModifyGroup, getGameState());
    #endif
    }

    void ScNetwork::removeGroup(int32_t id)
    {
    #ifndef DISABLE_NETWORK
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
    }

    void ScNetwork::kickPlayer(int32_t id)
    {
    #ifndef DISABLE_NETWORK
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
    }

    void ScNetwork::sendMessage(std::string message, DukValue players)
    {
    #ifndef DISABLE_NETWORK
        if (players.is_array())
        {
            if (Network::GetMode() == Network::Mode::server)
            {
                std::vector<uint8_t> playerIds;
                auto playerArray = players.as_array();
                for (const auto& item : playerArray)
                {
                    if (item.type() == DukValue::Type::NUMBER)
                    {
                        playerIds.push_back(static_cast<uint8_t>(item.as_uint()));
                    }
                }
                if (!playerArray.empty())
                {
                    Network::SendChat(message.c_str(), playerIds);
                }
            }
            else
            {
                duk_error(players.context(), DUK_ERR_ERROR, "Only servers can send private messages.");
            }
        }
        else
        {
            Network::SendChat(message.c_str());
        }
    #endif
    }

    #ifndef DISABLE_NETWORK
    std::shared_ptr<ScListener> ScNetwork::createListener()
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
        auto socket = std::make_shared<ScListener>(plugin);
        scriptEngine.AddSocket(socket);
        return socket;
    }
    #else
    void ScNetwork::createListener()
    {
        duk_error(_context, DUK_ERR_ERROR, "Networking has been disabled.");
    }
    #endif

    #ifndef DISABLE_NETWORK
    std::shared_ptr<ScSocket> ScNetwork::createSocket()
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
        auto socket = std::make_shared<ScSocket>(plugin);
        scriptEngine.AddSocket(socket);
        return socket;
    }
    #else
    void ScNetwork::createSocket()
    {
        duk_error(_context, DUK_ERR_ERROR, "Networking has been disabled.");
    }
    #endif

    void ScNetwork::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScNetwork::mode_get, nullptr, "mode");
        dukglue_register_property(ctx, &ScNetwork::numGroups_get, nullptr, "numGroups");
        dukglue_register_property(ctx, &ScNetwork::numPlayers_get, nullptr, "numPlayers");
        dukglue_register_property(ctx, &ScNetwork::groups_get, nullptr, "groups");
        dukglue_register_property(ctx, &ScNetwork::players_get, nullptr, "players");
        dukglue_register_property(ctx, &ScNetwork::currentPlayer_get, nullptr, "currentPlayer");
        dukglue_register_property(ctx, &ScNetwork::defaultGroup_get, &ScNetwork::defaultGroup_set, "defaultGroup");
        dukglue_register_property(ctx, &ScNetwork::stats_get, nullptr, "stats");
        dukglue_register_method(ctx, &ScNetwork::addGroup, "addGroup");
        dukglue_register_method(ctx, &ScNetwork::getGroup, "getGroup");
        dukglue_register_method(ctx, &ScNetwork::removeGroup, "removeGroup");
        dukglue_register_method(ctx, &ScNetwork::getPlayer, "getPlayer");
        dukglue_register_method(ctx, &ScNetwork::kickPlayer, "kickPlayer");
        dukglue_register_method(ctx, &ScNetwork::sendMessage, "sendMessage");

        dukglue_register_method(ctx, &ScNetwork::createListener, "createListener");
        dukglue_register_method(ctx, &ScNetwork::createSocket, "createSocket");
    }

} // namespace OpenRCT2::Scripting

#endif
