/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../ride/RideConstruction.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    struct TrackPlaceActionResult
    {
        uint8_t GroundFlags{ 0 };
    };

    class TrackPlaceAction final : public GameActionBase<GameCommand::PlaceTrack>
    {
    private:
        RideId _rideIndex{ RideId::GetNull() };
        TrackElemType _trackType{};
        ride_type_t _rideType{};
        CoordsXYZD _origin;
        int32_t _brakeSpeed{};
        int32_t _colour{};
        int32_t _seatRotation{};
        SelectedLiftAndInverted _trackPlaceFlags{};
        bool _fromTrackDesign{};

    public:
        TrackPlaceAction() = default;
        TrackPlaceAction(
            RideId rideIndex, TrackElemType trackType, ride_type_t rideType, const CoordsXYZD& origin, int32_t brakeSpeed,
            int32_t colour, int32_t seatRotation, SelectedLiftAndInverted liftHillAndAlternativeState, bool fromTrackDesign);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const final;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        bool CheckMapCapacity(int16_t numTiles) const;
    };
} // namespace OpenRCT2::GameActions
