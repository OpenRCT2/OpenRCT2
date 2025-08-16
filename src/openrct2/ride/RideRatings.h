/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../core/FixedPoint.hpp"
#include "../world/Location.hpp"

#include <array>

struct Ride;

using ride_rating = fixed16_2dp;
namespace OpenRCT2
{
    enum class TrackElemType : uint16_t;
}

// Convenience function for writing ride ratings. The result is a 16 bit signed
// integer. To create the ride rating 3.65 type MakeRideRating(3, 65).
constexpr ride_rating MakeRideRating(int16_t whole, uint8_t fraction)
{
    return MakeFixed2dp<ride_rating>(whole, fraction);
}
constexpr ride_rating kRideRatingUndefined = 0xFFFFu;

struct RideRatingUpdateState
{
    CoordsXYZ Proximity;
    CoordsXYZ ProximityStart;
    RideId CurrentRide;
    uint8_t State;
    OpenRCT2::TrackElemType ProximityTrackType;
    uint8_t ProximityBaseHeight;
    uint16_t ProximityTotal;
    uint16_t ProximityScores[26];
    uint16_t AmountOfBrakes;
    uint16_t AmountOfReversers;
    uint16_t StationFlags;
};

namespace OpenRCT2::RideRating
{
#pragma pack(push, 1)

    // Used for return values, for functions that modify all three.
    struct Tuple
    {
        ride_rating excitement{};
        ride_rating intensity{};
        ride_rating nausea{};

        bool isNull() const;
        void setNull();

        bool operator==(const Tuple& rhs) const = default;
    };
    static_assert(sizeof(Tuple) == 6);

#pragma pack(pop)

    static constexpr size_t kMaxUpdateStates = 4;
    using UpdateStates = std::array<RideRatingUpdateState, kMaxUpdateStates>;

    void ResetUpdateStates();
    void UpdateRide(const Ride& ride);
    void UpdateAll();
} // namespace OpenRCT2::RideRating

// Special Track Element Adjustment functions for RTDs
void SpecialTrackElementRatingsAjustment_Default(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);
void SpecialTrackElementRatingsAjustment_GhostTrain(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);
void SpecialTrackElementRatingsAjustment_LogFlume(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);
