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

DEFINE_GAME_ACTION(RideSetStatusAction, GAME_COMMAND_SET_RIDE_STATUS, GameActions::Result)
{
private:
    NetworkRideId_t _rideIndex{ RideIdNewNull };
    uint8_t _status{ RIDE_STATUS_CLOSED };

public:
    RideSetStatusAction() = default;
    RideSetStatusAction(ride_id_t rideIndex, uint8_t status)
        : _rideIndex(rideIndex)
        , _status(status)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
