/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_SCRIPTING__

#    include "../actions/NetworkModifyGroupAction.hpp"
#    include "../actions/PlayerKickAction.hpp"
#    include "../actions/PlayerSetGroupAction.hpp"
#    include "../network/NetworkAction.h"
#    include "../network/network.h"
#    include "Duktape.hpp"

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

        std::string name_get()
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

        std::vector<std::string> permissions_get()
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
                if (network_can_perform_action(index, permissionIndex))
                {
                    auto p = TransformPermissionKeyToJS(action.PermissionName);
                    result.push_back(p);
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
                auto toggle = (enabledPermissions[i] != (network_can_perform_action(groupIndex, (uint32_t)i) != 0));
                if (toggle)
                {
                    auto networkAction2 = NetworkModifyGroupAction(
                        ModifyGroupType::SetPermissions, _id, "", (uint32_t)i, PermissionState::Toggle);
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
        std::string TransformPermissionKeyToJS(const std::string& s)
        {
            auto result = s.substr(sizeof("PERMISSION_") - 1);
            for (auto& c : result)
            {
                c = std::tolower(c);
            }
            return result;
        }

        std::string TransformPermissionKeyToInternal(const std::string& s)
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

        int32_t id_get()
        {
            return _id;
        }

        std::string name_get()
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

        int32_t group_get()
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

        int32_t ping_get()
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

        int32_t commandsRan_get()
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

        int32_t moneySpent_get()
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

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScPlayer::id_get, nullptr, "id");
            dukglue_register_property(ctx, &ScPlayer::name_get, nullptr, "name");
            dukglue_register_property(ctx, &ScPlayer::group_get, &ScPlayer::group_set, "group");
            dukglue_register_property(ctx, &ScPlayer::ping_get, nullptr, "ping");
            dukglue_register_property(ctx, &ScPlayer::commandsRan_get, nullptr, "commandsRan");
            dukglue_register_property(ctx, &ScPlayer::moneySpent_get, nullptr, "moneySpent");
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

        std::string mode_get()
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
        int32_t players_get()
        {
#    ifndef DISABLE_NETWORK
            return network_get_num_players();
#    else
            return 0;
#    endif
        }
        int32_t groups_get()
        {
#    ifndef DISABLE_NETWORK
            return network_get_num_groups();
#    else
            return 0;
#    endif
        }
        int32_t defaultGroup_get()
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

        std::shared_ptr<ScPlayer> getPlayer(int32_t index)
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

        std::shared_ptr<ScPlayerGroup> getGroup(int32_t index)
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
                duk_error(players.context(), DUK_ERR_ERROR, "Not yet supported");
            }
            else
            {
                network_send_chat(message.c_str());
            }
#    endif
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScNetwork::mode_get, nullptr, "mode");
            dukglue_register_property(ctx, &ScNetwork::groups_get, nullptr, "groups");
            dukglue_register_property(ctx, &ScNetwork::players_get, nullptr, "players");
            dukglue_register_property(ctx, &ScNetwork::defaultGroup_get, &ScNetwork::defaultGroup_set, "defaultGroup");
            dukglue_register_method(ctx, &ScNetwork::getGroup, "getGroup");
            dukglue_register_method(ctx, &ScNetwork::getPlayer, "getPlayer");
            dukglue_register_method(ctx, &ScNetwork::kickPlayer, "kickPlayer");
            dukglue_register_method(ctx, &ScNetwork::sendMessage, "sendMessage");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
