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
constexpr const RideTypeDescriptor BoatHireRTD =
{
    SET_FIELD(Category, RIDE_CATEGORY_WATER),
    SET_FIELD(EnabledTrackPieces, (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_CURVE_VERY_SMALL)),
    SET_FIELD(ExtraTrackPieces, 0),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_boat_hire),
    SET_FIELD(Flags, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS | RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER |
                     RIDE_TYPE_FLAG_NO_TEST_MODE | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_CHECK_FOR_STALLING |
                     RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT),
    SET_FIELD(NameConvention, { RIDE_COMPONENT_TYPE_BOAT, RIDE_COMPONENT_TYPE_DOCKING_PLATFORM, RIDE_COMPONENT_TYPE_DOCKING_PLATFORM }),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(LiftData, { SoundId::Null, 5, 5 }),
};
// clang-format on
