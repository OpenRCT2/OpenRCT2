/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once

#include "GameAction.h"
namespace OpenRCT2
{
    class MazePlaceTrackAction final : public GameActionBase<GameCommand::PlaceMazeDesign>
    {
    private:
        CoordsXYZ _loc;
        RideId _rideIndex{ RideId::GetNull() };
        uint16_t _mazeEntry{};

    public:
        MazePlaceTrackAction() = default;
        MazePlaceTrackAction(const CoordsXYZ& location, RideId rideIndex, uint16_t mazeEntry);

        void AcceptParameters(GameActionParameterVisitor& visitor) override;
        void Serialise(DataSerialiser& stream) override;
        GameActions::Result Query() const override;
        GameActions::Result Execute() const override;
    };
} // namespace OpenRCT2
