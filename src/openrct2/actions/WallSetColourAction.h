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

DEFINE_GAME_ACTION(WallSetColourAction, GAME_COMMAND_SET_WALL_COLOUR, GameActions::Result)
{
private:
    CoordsXYZD _loc;
    int32_t _primaryColour{};
    int32_t _secondaryColour{};
    int32_t _tertiaryColour{};

public:
    WallSetColourAction() = default;
    WallSetColourAction(const CoordsXYZD& loc, int32_t primaryColour, int32_t secondaryColour, int32_t tertiaryColour);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
