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
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor AirPoweredVerticalCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL_STEEP) | (1ULL << TRACK_LIFT_HILL_CURVE) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_REVERSE_FREEFALL) | (1ULL << TRACK_SLOPE_TO_FLAT)),
    SET_FIELD(ExtraTrackPieces, (1ULL << TRACK_BOOSTER)),
    SET_FIELD(StartTrackPiece, TRACK_ELEM_END_STATION),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_air_powered_vertical_rc),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES),
    SET_FIELD(RideModes, (1ULL << RIDE_MODE_POWERED_LAUNCH_PASSTROUGH) | (1ULL << RIDE_MODE_POWERED_LAUNCH)),
    SET_FIELD(DefaultMode, RIDE_MODE_POWERED_LAUNCH_PASSTROUGH),
    SET_FIELD(NameConvention, { RIDE_COMPONENT_TYPE_TRAIN, RIDE_COMPONENT_TYPE_TRACK, RIDE_COMPONENT_TYPE_STATION }),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(LiftData, { SoundId::Null, 5, 5 }),
    SET_FIELD(UpkeepCosts, { 90, 20, 0, 0, 0, 10 }),
    SET_FIELD(PhotoItem, SHOP_ITEM_PHOTO),
};
// clang-format on
