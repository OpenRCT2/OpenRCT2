/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../RideData.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor AirPoweredVerticalCoasterRTD =
{
    SET_FIELD(EnabledTrackPieces, (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL_STEEP) | (1ULL << TRACK_LIFT_HILL_CURVE) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_REVERSE_FREEFALL) | (1ULL << TRACK_SLOPE_TO_FLAT)),
    SET_FIELD(ExtraTrackPieces, 0),
};
// clang-format on
