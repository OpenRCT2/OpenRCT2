/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../OpenRCT2.h"
#include "GameAction.h"

enum class ManagePlayerGuestMode : uint8_t
{
    Create = 0,
    Remove,
};

DEFINE_GAME_ACTION(ManagePlayerGuestAction, GAME_COMMAND_MANAGE_PLAYER_GUEST, GameActionResult)
{
private:
    uint8_t _mode{ 0 };
    NetworkPlayerId_t _player = -1;

public:
    ManagePlayerGuestAction()
    {
    }

    ManagePlayerGuestAction(ManagePlayerGuestMode mode, NetworkPlayerId_t playerId)
        : _mode(static_cast<uint8_t>(mode))
        , _player(playerId)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_mode) << DS_TAG(_player);
    }

    GameActionResult::Ptr Query() const override
    {
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto mode = static_cast<ManagePlayerGuestMode>(_mode);
        switch (mode)
        {
            case ManagePlayerGuestMode::Create:
                return CreateGuest();
            case ManagePlayerGuestMode::Remove:
                return RemoveGuest();
            default:
                break;
        }
        return std::make_unique<GameActionResult>();
    }

private:
    GameActionResult::Ptr CreateGuest() const
    {
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr RemoveGuest() const
    {
        return std::make_unique<GameActionResult>();
    }
};
