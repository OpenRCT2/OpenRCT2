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

#    include "../Context.h"
#    include "../actions/NetworkModifyGroupAction.h"
#    include "../actions/PlayerKickAction.h"
#    include "../actions/PlayerSetGroupAction.h"
#    include "../network/NetworkAction.h"
#    include "../network/network.h"
#    include "Duktape.hpp"
#    include "ScSocket.hpp"

namespace OpenRCT2::Scripting
{
    class ScPlayerGroup
    {
    private:
        int32_t _id;

    public:
        ScPlayerGroup(int32_t id)
            : _id(id)
        {
        }

        int32_t id_get()
        {
            return _id;
        }

        std::string name_get() const
        {
#    ifndef DISABLE_NETWORK
            auto index = network_get_group_index(_id);
            if (index == -1)
                return {};
            return network_get_group_name(index);
#    else
            return {};
#    endif
        }
        void name_set(std::string value)
        {
#    ifndef DISABLE_NETWORK
            auto action = NetworkModifyGroupAction(ModifyGroupType::SetName, _id, value);
            GameActions::Execute(&action);
#    endif
        }

        std::vector<std::string> permissions_get() const
        {
#    ifndef DISABLE_NETWORK
            auto index = network_get_group_index(_id);
            if (index == -1)
                return {};

            // Create array of permissions
            std::vector<std::string> result;
            auto permissionIndex = 0;
            for (const auto& action : NetworkActions::Actions)
            {
                if (network_can_perform_action(index, static_cast<NetworkPermission>(permissionIndex)))
                {
                    result.push_back(TransformPermissionKeyToJS(action.PermissionName));
                }
                permissionIndex++;
            }
            return result;
#    else
            return {};
#    endif
        }
        void permissions_set(std::vector<std::string> value)
        {
#    ifndef DISABLE_NETWORK
            auto groupIndex = network_get_group_index(_id);
            if (groupIndex == -1)
                return;

            // First clear all permissions
            auto networkAction = NetworkModifyGroupAction(
                ModifyGroupType::SetPermissions, _id, "", 0, PermissionState::ClearAll);
            GameActions::Execute(&networkAction);

            std::vector<bool> enabledPermissions;
            enabledPermissions.resize(NetworkActions::Actions.size());
            for (const auto& p : value)
            {
                auto permissionName = TransformPermissionKeyToInternal(p);

                auto permissionIndex = 0;
                for (const auto& action : NetworkActions::Actions)
                {
                    if (action.PermissionName == permissionName)
                    {
                        enabledPermissions[permissionIndex] = true;
                    }
                    permissionIndex++;
                }
            }

            for (size_t i = 0; i < enabledPermissions.size(); i++)
            {
                auto toggle
                    = (enabledPermissions[i]
                       != (network_can_perform_action(groupIndex, static_cast<NetworkPermission>(i)) != 0));
                if (toggle)
                {
                    auto networkAction2 = NetworkModifyGroupAction(
                        ModifyGroupType::SetPermissions, _id, "", static_cast<uint32_t>(i), PermissionState::Toggle);
                    GameActions::Execute(&networkAction2);
                }
            }
#    endif
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScPlayerGroup::id_get, nullptr, "id");
            dukglue_register_property(ctx, &ScPlayerGroup::name_get, &ScPlayerGroup::name_set, "name");
            dukglue_register_property(ctx, &ScPlayerGroup::permissions_get, &ScPlayerGroup::permissions_set, "permissions");
        }

    private:
        static std::string TransformPermissionKeyToJS(const std::string& s)
        {
            auto result = s.substr(sizeof("PERMISSION_") - 1);
            for (auto& c : result)
            {
                c = std::tolower(c);
            }
            return result;
        }

        static std::string TransformPermissionKeyToInternal(const std::string& s)
        {
            auto result = "PERMISSION_" + s;
            for (auto& c : result)
            {
                c = std::toupper(c);
            }
            return result;
        }
    };

    class ScPlayer
    {
    private:
        int32_t _id;

    public:
        ScPlayer(int32_t id)
            : _id(id)
        {
        }

        int32_t id_get() const
        {
            return _id;
        }

        std::string name_get() const
        {
#    ifndef DISABLE_NETWORK
            auto index = network_get_player_index(_id);
            if (index == -1)
                return {};
            return network_get_player_name(index);
#    else
            return {};
#    endif
        }

        int32_t group_get() const
        {
#    ifndef DISABLE_NETWORK
            auto index = network_get_player_index(_id);
            if (index == -1)
                return {};
            return network_get_player_group(index);
#    else
            return 0;
#    endif
        }
        void group_set(int32_t value)
        {
#    ifndef DISABLE_NETWORK
            auto playerSetGroupAction = PlayerSetGroupAction(_id, value);
            GameActions::Execute(&playerSetGroupAction);
#    endif
        }

        int32_t ping_get() const
        {
#    ifndef DISABLE_NETWORK
            auto index = network_get_player_index(_id);
            if (index == -1)
                return {};
            return network_get_player_ping(index);
#    else
            return 0;
#    endif
        }

        int32_t commandsRan_get() const
        {
#    ifndef DISABLE_NETWORK
            auto index = network_get_player_index(_id);
            if (index == -1)
                return {};
            return network_get_player_commands_ran(index);
#    else
            return 0;
#    endif
        }

        int32_t moneySpent_get() const
        {
#    ifndef DISABLE_NETWORK
            auto index = network_get_player_index(_id);
            if (index == -1)
                return {};
            return network_get_player_money_spent(index);
#    else
            return 0;
#    endif
        }

        std::string ipAddress_get() const
        {
            return network_get_player_ip_address(_id);
        }

        std::string publicKeyHash_get() const
        {
            return network_get_player_public_key_hash(_id);
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScPlayer::id_get, nullptr, "id");
            dukglue_register_property(ctx, &ScPlayer::name_get, nullptr, "name");
            dukglue_register_property(ctx, &ScPlayer::group_get, &ScPlayer::group_set, "group");
            dukglue_register_property(ctx, &ScPlayer::ping_get, nullptr, "ping");
            dukglue_register_property(ctx, &ScPlayer::commandsRan_get, nullptr, "commandsRan");
            dukglue_register_property(ctx, &ScPlayer::moneySpent_get, nullptr, "moneySpent");
            dukglue_register_property(ctx, &ScPlayer::ipAddress_get, nullptr, "ipAddress");
            dukglue_register_property(ctx, &ScPlayer::publicKeyHash_get, nullptr, "publicKeyHash");
        }
    };

    class ScNetwork
    {
    private:
#    ifdef __clang__
        [[maybe_unused]]
#    endif
        duk_context* _context;

    public:
        ScNetwork(duk_context* ctx)
            : _context(ctx)
        {
        }

        std::string mode_get() const
        {
#    ifndef DISABLE_NETWORK
            switch (network_get_mode())
            {
                case NETWORK_MODE_SERVER:
                    return "server";
                case NETWORK_MODE_CLIENT:
                    return "client";
            }
#    endif
            return "none";
        }
        int32_t numPlayers_get() const
        {
#    ifndef DISABLE_NETWORK
            return network_get_num_players();
#    else
            return 0;
#    endif
        }
        int32_t numGroups_get() const
        {
#    ifndef DISABLE_NETWORK
            return network_get_num_groups();
#    else
            return 0;
#    endif
        }
        int32_t defaultGroup_get() const
        {
#    ifndef DISABLE_NETWORK
            return network_get_default_group();
#    else
            return 0;
#    endif
        }
        void defaultGroup_set(int32_t value)
        {
#    ifndef DISABLE_NETWORK
            auto action = NetworkModifyGroupAction(ModifyGroupType::SetDefault, value);
            GameActions::Execute(&action);
#    endif
        }

        std::vector<std::shared_ptr<ScPlayerGroup>> groups_get() const
        {
            std::vector<std::shared_ptr<ScPlayerGroup>> groups;
#    ifndef DISABLE_NETWORK
            auto numGroups = network_get_num_groups();
            for (int32_t i = 0; i < numGroups; i++)
            {
                auto groupId = network_get_group_id(i);
                groups.push_back(std::make_shared<ScPlayerGroup>(groupId));
            }
#    endif
            return groups;
        }

        std::vector<std::shared_ptr<ScPlayer>> players_get() const
        {
            std::vector<std::shared_ptr<ScPlayer>> players;
#    ifndef DISABLE_NETWORK
            auto numPlayers = network_get_num_players();
            for (int32_t i = 0; i < numPlayers; i++)
            {
                auto playerId = network_get_player_id(i);
                players.push_back(std::make_shared<ScPlayer>(playerId));
            }
#    endif
            return players;
        }

        std::shared_ptr<ScPlayer> currentPlayer_get() const
        {
            std::shared_ptr<ScPlayer> player;
#    ifndef DISABLE_NETWORK
            auto playerId = network_get_current_player_id();
            player = std::make_shared<ScPlayer>(playerId);
#    endif
            return player;
        }

        std::shared_ptr<ScPlayer> getPlayer(int32_t index) const
        {
#    ifndef DISABLE_NETWORK
            auto numPlayers = network_get_num_players();
            if (index < numPlayers)
            {
                auto playerId = network_get_player_id(index);
                return std::make_shared<ScPlayer>(playerId);
            }
#    endif
            return nullptr;
        }

        DukValue stats_get() const
        {
#    ifndef DISABLE_NETWORK
            auto obj = OpenRCT2::Scripting::DukObject(_context);
            auto networkStats = network_get_stats();
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
#    else
            return ToDuk(_context, nullptr);
#    endif
        }

        std::shared_ptr<ScPlayerGroup> getGroup(int32_t index) const
        {
#    ifndef DISABLE_NETWORK
            auto numGroups = network_get_num_groups();
            if (index < numGroups)
            {
                auto groupId = network_get_group_id(index);
                return std::make_shared<ScPlayerGroup>(groupId);
            }
#    endif
            return nullptr;
        }

        void addGroup()
        {
#    ifndef DISABLE_NETWORK
            auto networkModifyGroup = NetworkModifyGroupAction(ModifyGroupType::AddGroup);
            GameActions::Execute(&networkModifyGroup);
#    endif
        }

        void removeGroup(int32_t index)
        {
#    ifndef DISABLE_NETWORK
            auto numGroups = network_get_num_groups();
            if (index < numGroups)
            {
                auto groupId = network_get_group_id(index);
                auto networkAction = NetworkModifyGroupAction(ModifyGroupType::RemoveGroup, groupId);
                GameActions::Execute(&networkAction);
            }
#    endif
        }

        void kickPlayer(int32_t index)
        {
#    ifndef DISABLE_NETWORK
            auto numPlayers = network_get_num_players();
            if (index < numPlayers)
            {
                auto playerId = network_get_player_id(index);
                auto kickPlayerAction = PlayerKickAction(playerId);
                GameActions::Execute(&kickPlayerAction);
            }
#    endif
        }

        void sendMessage(std::string message, DukValue players)
        {
#    ifndef DISABLE_NETWORK
            if (players.is_array())
            {
                if (network_get_mode() == NETWORK_MODE_SERVER)
                {
                    std::vector<uint8_t> playerIds;
                    auto playerArray = players.as_array();
                    for (const auto& item : playerArray)
                    {
                        if (item.type() == DukValue::Type::NUMBER)
                        {
                            playerIds.push_back(static_cast<uint8_t>(item.as_int()));
                        }
                    }
                    if (!playerArray.empty())
                    {
                        network_send_chat(message.c_str(), playerIds);
                    }
                }
                else
                {
                    duk_error(players.context(), DUK_ERR_ERROR, "Only servers can send private messages.");
                }
            }
            else
            {
                network_send_chat(message.c_str());
            }
#    endif
        }

#    ifndef DISABLE_NETWORK
        std::shared_ptr<ScListener> createListener()
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            auto socket = std::make_shared<ScListener>(plugin);
            scriptEngine.AddSocket(socket);
            return socket;
        }
#    else
        void createListener()
        {
            duk_error(_context, DUK_ERR_ERROR, "Networking has been disabled.");
        }
#    endif

#    ifndef DISABLE_NETWORK
        std::shared_ptr<ScSocket> createSocket()
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            auto socket = std::make_shared<ScSocket>(plugin);
            scriptEngine.AddSocket(socket);
            return socket;
        }
#    else
        void createSocket()
        {
            duk_error(_context, DUK_ERR_ERROR, "Networking has been disabled.");
        }
#    endif

        static void Register(duk_context* ctx)
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
    };
} // namespace OpenRCT2::Scripting

#endif
