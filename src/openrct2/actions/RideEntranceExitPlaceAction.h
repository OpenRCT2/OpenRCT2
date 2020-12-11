/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Entrance.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(RideEntranceExitPlaceAction, GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, GameActions::Result)
{
private:
    CoordsXY _loc;
    Direction _direction{ INVALID_DIRECTION };
    NetworkRideId_t _rideIndex{ RIDE_ID_NULL };
    StationIndex _stationNum{ STATION_INDEX_NULL };
    bool _isExit{};

public:
    RideEntranceExitPlaceAction() = default;
    RideEntranceExitPlaceAction(
        const CoordsXY& loc, Direction direction, ride_id_t rideIndex, StationIndex stationNum, bool isExit);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

    static GameActions::Result::Ptr TrackPlaceQuery(const CoordsXYZ& loc, const bool isExit);
};
