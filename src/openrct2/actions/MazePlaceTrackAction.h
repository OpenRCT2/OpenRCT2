/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

namespace OpenRCT2::GameActions
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

        void AcceptParameters(GameActionParameterVisitor&) final;
        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
