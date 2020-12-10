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

DEFINE_GAME_ACTION(RideSetNameAction, GAME_COMMAND_SET_RIDE_NAME, GameActions::Result)
{
private:
    NetworkRideId_t _rideIndex{ RideIdNewNull };
    std::string _name;

public:
    RideSetNameAction() = default;
    RideSetNameAction(ride_id_t rideIndex, const std::string& name)
        : _rideIndex(rideIndex)
        , _name(name)
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
