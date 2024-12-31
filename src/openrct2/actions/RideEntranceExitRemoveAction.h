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

class RideEntranceExitRemoveAction final : public GameActionBase<GameCommand::RemoveRideEntranceOrExit>
{
private:
    CoordsXY _loc;
    RideId _rideIndex{ RideId::GetNull() };
    StationIndex _stationNum{ StationIndex::GetNull() };
    bool _isExit{};

public:
    RideEntranceExitRemoveAction() = default;
    RideEntranceExitRemoveAction(const CoordsXY& loc, RideId rideIndex, StationIndex stationNum, bool isExit);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
