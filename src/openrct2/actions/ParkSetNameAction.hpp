/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../config/Config.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "GameAction.h"

struct ParkSetNameAction : public GameActionBase<GAME_COMMAND_SET_PARK_NAME, GameActionResult>
{
private:
    std::string _name;

public:
    ParkSetNameAction() {}
    ParkSetNameAction(const std::string& name)
        : _name(name)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _name;
    }

    GameActionResult::Ptr Query() const override
    {
        if (_name.empty())
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_RENAME_PARK, STR_INVALID_NAME_FOR_PARK);
        }

        // TODO create a version of user_string_allocate that only tests so we do not have to free it straight afterwards
        auto stringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, _name.c_str());
        if (stringId == 0)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_RENAME_PARK, STR_INVALID_NAME_FOR_PARK);
        }
        user_string_free(stringId);

        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        // Do a no-op if new name is the same as the current name is the same
        std::string oldName = GetCurrentParkName();
        if (_name == oldName)
        {
            return MakeResult();
        }

        // Allocate new string for park name
        auto newNameId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, _name.c_str());
        if (newNameId == 0)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_RENAME_PARK, STR_INVALID_NAME_FOR_PARK);
        }

        // Replace park name with new string id
        user_string_free(gParkName);
        gParkName = newNameId;

        // Log park rename command if we are in multiplayer and logging is enabled
        if ((network_get_mode() == NETWORK_MODE_CLIENT ||
             network_get_mode() == NETWORK_MODE_SERVER) && gConfigNetwork.log_server_actions)
        {
            LogAction(oldName);
        }

        gfx_invalidate_screen();
        return MakeResult();
    }

private:
    std::string GetCurrentParkName() const
    {
        char buffer[128];
        format_string(buffer, sizeof(buffer), gParkName, &gParkNameArgs);
        return buffer;
    }

    void LogAction(const std::string &oldName) const
    {
        // Get player name
        auto playerIndex = network_get_player_index(game_command_playerid);
        auto playerName = network_get_player_name(playerIndex);

        char logMessage[256];
        const char * args[3] = {
            playerName,
            oldName.c_str(),
            _name.c_str()
        };
        format_string(logMessage, sizeof(logMessage), STR_LOG_PARK_NAME, (void *)args);
        network_append_server_log(logMessage);
    }
};
