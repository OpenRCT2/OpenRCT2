/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScNetwork.hpp"

    #include "../../../Context.h"
    #include "../../../actions/NetworkModifyGroupAction.h"
    #include "../../../actions/PlayerKickAction.h"
    #include "../../../network/NetworkAction.h"
    #include "../../../network/network.h"

namespace OpenRCT2::Scripting
{
    ScNetwork::ScNetwork(duk_context* ctx)
        : _context(ctx)
    {
    }

    std::string ScNetwork::mode_get() const
    {
    #ifndef DISABLE_NETWORK
        switch (NetworkGetMode())
        {
            case NETWORK_MODE_SERVER:
                return "server";
            case NETWORK_MODE_CLIENT:
                return "client";
        }
    #endif
        return "none";
    }

    int32_t ScNetwork::numPlayers_get() const
    {
    #ifndef DISABLE_NETWORK
        return NetworkGetNumPlayers();
    #else
        return 0;
    #endif
    }

    int32_t ScNetwork::numGroups_get() const
    {
    #ifndef DISABLE_NETWORK
        return NetworkGetNumGroups();
    #else
        return 0;
    #endif
    }

    int32_t ScNetwork::defaultGroup_get() const
    {
    #ifndef DISABLE_NETWORK
        return NetworkGetDefaultGroup();
    #else
        return 0;
    #endif
    }

    void ScNetwork::defaultGroup_set(int32_t value)
    {
    #ifndef DISABLE_NETWORK
        auto action = NetworkModifyGroupAction(ModifyGroupType::SetDefault, value);
        GameActions::Execute(&action);
    #endif
    }

    std::vector<std::shared_ptr<ScPlayerGroup>> ScNetwork::groups_get() const
    {
        std::vector<std::shared_ptr<ScPlayerGroup>> groups;
    #ifndef DISABLE_NETWORK
        auto numGroups = NetworkGetNumGroups();
        for (int32_t i = 0; i < numGroups; i++)
        {
            auto groupId = NetworkGetGroupID(i);
            groups.push_back(std::make_shared<ScPlayerGroup>(groupId));
        }
    #endif
        return groups;
    }

    std::vector<std::shared_ptr<ScPlayer>> ScNetwork::players_get() const
    {
        std::vector<std::shared_ptr<ScPlayer>> players;
    #ifndef DISABLE_NETWORK
        auto numPlayers = NetworkGetNumPlayers();
        for (int32_t i = 0; i < numPlayers; i++)
        {
            auto playerId = NetworkGetPlayerID(i);
            players.push_back(std::make_shared<ScPlayer>(playerId));
        }
    #endif
        return players;
    }

    std::shared_ptr<ScPlayer> ScNetwork::currentPlayer_get() const
    {
        std::shared_ptr<ScPlayer> player;
    #ifndef DISABLE_NETWORK
        auto playerId = NetworkGetCurrentPlayerId();
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
        return nullptr;
    }

    DukValue ScNetwork::stats_get() const
    {
    #ifndef DISABLE_NETWORK
        auto obj = OpenRCT2::Scripting::DukObject(_context);
        auto networkStats = NetworkGetStats();
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
            auto numGroups = NetworkGetNumGroups();
            if (index < numGroups)
            {
                auto groupId = NetworkGetGroupID(index);
                return std::make_shared<ScPlayerGroup>(groupId);
            }
        }
        else
        {
            auto index = NetworkGetGroupIndex(id);
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
        auto networkModifyGroup = NetworkModifyGroupAction(ModifyGroupType::AddGroup);
        GameActions::Execute(&networkModifyGroup);
    #endif
    }

    void ScNetwork::removeGroup(int32_t id)
    {
    #ifndef DISABLE_NETWORK
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
    }

    void ScNetwork::kickPlayer(int32_t id)
    {
    #ifndef DISABLE_NETWORK
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
    }

    void ScNetwork::sendMessage(std::string message, DukValue players)
    {
    #ifndef DISABLE_NETWORK
        if (players.is_array())
        {
            if (NetworkGetMode() == NETWORK_MODE_SERVER)
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
                    NetworkSendChat(message.c_str(), playerIds);
                }
            }
            else
            {
                duk_error(players.context(), DUK_ERR_ERROR, "Only servers can send private messages.");
            }
        }
        else
        {
            NetworkSendChat(message.c_str());
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
