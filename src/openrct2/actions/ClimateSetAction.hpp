/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Climate.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(ClimateSetAction, GAME_COMMAND_SET_CLIMATE, GameActionResult)
{
private:
    uint8_t _climate;

public:
    ClimateSetAction() = default;
    ClimateSetAction(ClimateType climate)
        : _climate(static_cast<uint8_t>(climate))
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_climate);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_climate >= static_cast<uint8_t>(ClimateType::Count))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_CLIMATE_ID, STR_NONE);
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        gClimate = ClimateType{ _climate };

        gfx_invalidate_screen();

        return std::make_unique<GameActionResult>();
    }
};
