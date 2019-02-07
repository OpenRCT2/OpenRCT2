/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"


DEFINE_GAME_ACTION(PauseToggleAction, GAME_COMMAND_TOGGLE_PAUSE, GameActionResult)
{
public:
    PauseToggleAction(){}

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    GameActionResult::Ptr Query() const override
    {
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        pause_toggle();
        return std::make_unique<GameActionResult>();
    }
};
