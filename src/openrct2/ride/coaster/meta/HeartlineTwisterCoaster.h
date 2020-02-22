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
constexpr const RideTypeDescriptor HeartlineTwisterCoasterRTD =
{
    SET_FIELD(EnabledTrackPieces, (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_LIFT_HILL_STEEP) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_HEARTLINE_ROLL) | (1ULL << TRACK_HEARTLINE_TRANSFER)),
    SET_FIELD(ExtraTrackPieces, 0),
};
// clang-format on
