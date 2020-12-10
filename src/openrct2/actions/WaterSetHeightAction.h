/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

DEFINE_GAME_ACTION(WaterSetHeightAction, GAME_COMMAND_SET_WATER_HEIGHT, GameActions::Result)
{
private:
    CoordsXY _coords;
    uint8_t _height{};

public:
    WaterSetHeightAction() = default;
    WaterSetHeightAction(const CoordsXY& coords, uint8_t height)
        : _coords(coords)
        , _height(height)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    rct_string_id CheckParameters() const;
};
