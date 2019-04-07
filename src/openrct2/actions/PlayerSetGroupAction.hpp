/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../network/NetworkGroup.h"
#include "../network/NetworkPlayer.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(PlayerSetGroupAction, GAME_COMMAND_SET_PLAYER_GROUP, GameActionResult)
{
private:
    uint8_t _playerId{ std::numeric_limits<uint8_t>::max() };
    uint8_t _groupId{ std::numeric_limits<uint8_t>::max() };

public:
    PlayerSetGroupAction()
    {
    }

    PlayerSetGroupAction(uint8_t playerId, uint8_t groupId)
        : _playerId(playerId)
        , _groupId(groupId)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_playerId) << DS_TAG(_groupId);
    }
    GameActionResult::Ptr Query() const override
    {
        return QueryExecute(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return QueryExecute(true);
    }

private:
#ifndef DISABLE_NETWORK
    GameActionResult::Ptr QueryExecute(bool isExecuting) const
    {
        NetworkPlayer* player = gNetwork.GetPlayerByID(_playerid);

        NetworkGroup* fromgroup = gNetwork.GetGroupByID(GetPlayer());
        if (player == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_DO_THIS);
        }

        if (!gNetwork.GetGroupByID(_groupId))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_DO_THIS);
        }

        if (player->Flags & NETWORK_PLAYER_FLAG_ISSERVER)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_GROUP_THAT_THE_HOST_BELONGS_TO);
        }

        if (_groupId == 0 && fromgroup && fromgroup->Id != 0)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_SET_TO_THIS_GROUP);
        }

        if (isExecuting)
        {
            player->Group = _groupId;

            if (network_get_mode() == NETWORK_MODE_SERVER)
            {
                // Add or update saved user
                NetworkUserManager* userManager = &gNetwork._userManager;
                NetworkUser* networkUser = userManager->GetOrAddUser(player->KeyHash);
                networkUser->GroupId = _groupId;
                networkUser->Name = player->Name;
                userManager->Save();
            }

            window_invalidate_by_number(WC_PLAYER, _playerId);

            // Log set player group event
            NetworkPlayer* game_command_player = gNetwork.GetPlayerByID(GetPlayer());
            NetworkGroup* new_player_group = gNetwork.GetGroupByID(_groupId);
            char log_msg[256];
            const char* args[3] = {
                player->Name.c_str(),
                new_player_group->GetName().c_str(),
                game_command_player->Name.c_str(),
            };
            format_string(log_msg, 256, STR_LOG_SET_PLAYER_GROUP, args);
            network_append_server_log(log_msg);
        }
        return MakeResult();
    }
#else
    GameActionResult::Ptr QueryExecute(bool isExecuting) const
    {
        return MakeResult();
    }
#endif
};
