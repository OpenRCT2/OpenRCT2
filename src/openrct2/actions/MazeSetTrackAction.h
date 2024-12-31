/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

class MazeSetTrackAction final : public GameActionBase<GameCommand::SetMazeTrack>
{
private:
    CoordsXYZD _loc;
    bool _initialPlacement{};
    RideId _rideIndex{ RideId::GetNull() };
    uint8_t _mode{};

public:
    MazeSetTrackAction() = default;
    MazeSetTrackAction(const CoordsXYZD& location, bool initialPlacement, RideId rideIndex, uint8_t mode);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;
    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    uint8_t MazeGetSegmentBit(const CoordsXY&) const;
};
