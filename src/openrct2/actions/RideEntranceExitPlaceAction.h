/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Entrance.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    class RideEntranceExitPlaceAction final : public GameActionBase<GameCommand::PlaceRideEntranceOrExit>
    {
    private:
        CoordsXY _loc;
        Direction _direction{ kInvalidDirection };
        RideId _rideIndex{ RideId::GetNull() };
        StationIndex _stationNum{ StationIndex::GetNull() };
        bool _isExit{};

    public:
        RideEntranceExitPlaceAction() = default;
        RideEntranceExitPlaceAction(
            const CoordsXY& loc, Direction direction, RideId rideIndex, StationIndex stationNum, bool isExit);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

        static Result TrackPlaceQuery(GameState_t& gameState, const CoordsXYZ& loc, bool isExit);
    };
} // namespace OpenRCT2::GameActions
