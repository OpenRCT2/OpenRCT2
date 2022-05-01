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
constexpr const RideTypeDescriptor SpinningWildMouseRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT,TRACK_STATION_END,TRACK_LIFT_HILL,  TRACK_SLOPE,TRACK_SLOPE_LONG,TRACK_CURVE_VERY_SMALL,TRACK_CURVE_SMALL, TRACK_BRAKES,TRACK_BLOCK_BRAKES,TRACK_ROTATION_CONTROL_TOGGLE}),
    SET_FIELD(ExtraTrackPieces, {TRACK_LIFT_HILL_STEEP, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_wild_mouse),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_SPINNING_WILD_MOUSE_GROUP, STR_SPINNING_WILD_MOUSE_GROUP_DESC }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_SPINNING_WILD_MOUSE)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE)),
    SET_FIELD(Heights, { 16, 24, 4, 7, }),
    SET_FIELD(MaxMass, 4),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftWildMouse, 4, 6 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_wild_mouse),
    SET_FIELD(RatingsMultipliers, { 50, 30, 30 }),
    SET_FIELD(UpkeepCosts, { 40, 20, 80, 9, 3, 10 }),
    SET_FIELD(BuildCosts, { 27.50_GBP, 2.00_GBP, 40, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_1),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 55),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_YELLOW },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_WHITE },
        { COLOUR_DARK_PURPLE, COLOUR_BRIGHT_PINK, COLOUR_DARK_PURPLE },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_BORDEAUX_RED },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_WILD_MOUSE_TRACK, SPR_RIDE_DESIGN_PREVIEW_WILD_MOUSE_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(StartRideMusic, OpenRCT2::RideAudio::DefaultStartRideMusicChannel),
    SET_FIELD(Name, "spinning_wild_mouse"),
};
// clang-format on
