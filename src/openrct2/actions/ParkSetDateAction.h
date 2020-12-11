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

DEFINE_GAME_ACTION(ParkSetDateAction, GAME_COMMAND_SET_DATE, GameActions::Result)
{
private:
    int32_t _year{};
    int32_t _month{};
    int32_t _day{};

public:
    ParkSetDateAction() = default;
    ParkSetDateAction(int32_t year, int32_t month, int32_t day);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
