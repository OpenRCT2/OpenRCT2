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

DEFINE_GAME_ACTION(ParkMarketingAction, GAME_COMMAND_START_MARKETING_CAMPAIGN, GameActions::Result)
{
private:
    int32_t _type{};
    int32_t _item{};
    int32_t _numWeeks{};

    GameActions::Result::Ptr CreateResult() const;
    money32 CalculatePrice() const;

public:
    ParkMarketingAction() = default;
    ParkMarketingAction(int32_t type, int32_t item, int32_t numWeeks)
        : _type(type)
        , _item(item)
        , _numWeeks(numWeeks)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override;

    GameActions::Result::Ptr Query() const override;

    GameActions::Result::Ptr Execute() const override;
};
