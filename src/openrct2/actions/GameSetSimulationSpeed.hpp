/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

// Clang format is broken for small game actions
// clang-format off
DEFINE_GAME_ACTION(GameSetSimulationSpeedAction, GAME_COMMAND_SET_SIMULATION_SPEED, GameActionResult)
{
private:
    uint32_t _simSpeed;
public:
    GameSetSimulationSpeedAction(int32_t simSpeed = 1): _simSpeed(simSpeed){};

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    GameActionResult::Ptr Query() const override
    {
        return std::make_unique<GameActionResult>();
    }

    uint32_t GetSimulationSpeed() const
    {
        return _simSpeed;
    }

     void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_simSpeed);
    }

    GameActionResult::Ptr Execute() const override
    {
        if (gGamePaused) {
            pause_toggle();
        }
        gGameSpeed = _simSpeed;
        window_invalidate_by_class(WC_TOP_TOOLBAR);
        return std::make_unique<GameActionResult>();
    }
};
// clang-format on
