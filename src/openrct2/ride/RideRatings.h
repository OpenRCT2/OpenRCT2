/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FixedPoint.hpp"
#include "../core/Money.hpp"
#include "../world/Location.hpp"
#include "RideTypes.h"

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

#pragma pack(push, 1)

// Used for return values, for functions that modify all three.
struct RatingTuple
{
    ride_rating excitement{};
    ride_rating intensity{};
    ride_rating nausea{};

    bool isNull() const;
    void setNull();
};
static_assert(sizeof(RatingTuple) == 6);

#pragma pack(pop)

enum
{
    RIDE_RATING_STATION_FLAG_NO_ENTRANCE = 1 << 0
};

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

static constexpr size_t kRideRatingMaxUpdateStates = 4;
using RideRatingUpdateStates = std::array<RideRatingUpdateState, kRideRatingMaxUpdateStates>;

void RideRatingResetUpdateStates();

void RideRatingsUpdateRide(const Ride& ride);
void RideRatingsUpdateAll();

// Special Track Element Adjustment functions for RTDs
void SpecialTrackElementRatingsAjustment_Default(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);
void SpecialTrackElementRatingsAjustment_GhostTrain(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);
void SpecialTrackElementRatingsAjustment_LogFlume(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);
