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

DEFINE_GAME_ACTION(StaffSetColourAction, GAME_COMMAND_SET_STAFF_COLOUR, GameActions::Result)
{
private:
    uint8_t _staffType{};
    uint8_t _colour{};

public:
    StaffSetColourAction() = default;
    StaffSetColourAction(StaffType staffType, uint8_t colour);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
