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
    struct TrackPlaceActionResult
    {
        uint8_t GroundFlags{ 0 };
    };

    class TrackPlaceAction final : public GameActionBase<GameCommand::PlaceTrack>
    {
    private:
        RideId _rideIndex{ RideId::GetNull() };
        int32_t _trackType{};
        ride_type_t _rideType{};
        CoordsXYZD _origin;
        int32_t _brakeSpeed{};
        int32_t _colour{};
        int32_t _seatRotation{};
        int32_t _trackPlaceFlags{};
        bool _fromTrackDesign{};

    public:
        TrackPlaceAction() = default;
        TrackPlaceAction(
            RideId rideIndex, int32_t trackType, ride_type_t rideType, const CoordsXYZD& origin, int32_t brakeSpeed,
            int32_t colour, int32_t seatRotation, int32_t liftHillAndAlternativeState, bool fromTrackDesign);

        void AcceptParameters(GameActionParameterVisitor& visitor) override;

        uint16_t GetActionFlags() const override final;

        void Serialise(DataSerialiser& stream) override;
        GameActions::Result Query() const override;
        GameActions::Result Execute() const override;

    private:
        bool CheckMapCapacity(int16_t numTiles) const;
    };
} // namespace OpenRCT2
