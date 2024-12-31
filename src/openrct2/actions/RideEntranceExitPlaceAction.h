/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Entrance.h"
#include "GameAction.h"

class RideEntranceExitPlaceAction final : public GameActionBase<GameCommand::PlaceRideEntranceOrExit>
{
private:
    CoordsXY _loc;
    Direction _direction{ INVALID_DIRECTION };
    RideId _rideIndex{ RideId::GetNull() };
    StationIndex _stationNum{ StationIndex::GetNull() };
    bool _isExit{};

public:
    RideEntranceExitPlaceAction() = default;
    RideEntranceExitPlaceAction(
        const CoordsXY& loc, Direction direction, RideId rideIndex, StationIndex stationNum, bool isExit);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

    static OpenRCT2::GameActions::Result TrackPlaceQuery(const CoordsXYZ& loc, const bool isExit);
};
