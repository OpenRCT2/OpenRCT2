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

DEFINE_GAME_ACTION(ClimateSetAction, GAME_COMMAND_SET_CLIMATE, GameActions::Result)
{
private:
    ClimateType _climate{};

public:
    ClimateSetAction() = default;
    ClimateSetAction(ClimateType climate)
        : _climate(climate)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_climate);
    }

    GameActions::Result::Ptr Query() const override
    {
        if (_climate >= ClimateType::Count)
        {
            return std::make_unique<GameActions::Result>(
                GameActions::Status::InvalidParameters, STR_INVALID_CLIMATE_ID, STR_NONE);
        }

        return std::make_unique<GameActions::Result>();
    }

    GameActions::Result::Ptr Execute() const override
    {
        gClimate = ClimateType{ _climate };

        gfx_invalidate_screen();

        return std::make_unique<GameActions::Result>();
    }
};
