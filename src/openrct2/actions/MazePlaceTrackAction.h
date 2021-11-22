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

class MazePlaceTrackAction final : public GameActionBase<GameCommand::PlaceMazeDesign>
{
private:
    CoordsXYZ _loc;
    NetworkRideId_t _rideIndex{ RIDE_ID_NULL };
    uint16_t _mazeEntry{};

public:
    MazePlaceTrackAction() = default;
    MazePlaceTrackAction(const CoordsXYZ& location, NetworkRideId_t rideIndex, uint16_t mazeEntry);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;
    void Serialise(DataSerialiser& stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
