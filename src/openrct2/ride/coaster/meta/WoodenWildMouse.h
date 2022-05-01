/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor WoodenWildMouseRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_LIFT_HILL_STEEP, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_LONG, TRACK_CURVE_VERY_SMALL, TRACK_CURVE_SMALL}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_wooden_wild_mouse),
    SET_FIELD(Flags, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT |
                     RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION | RIDE_TYPE_FLAG_ALLOW_DOORS_ON_TRACK |
                     RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_WOODEN_WILD_MOUSE, STR_RIDE_DESCRIPTION_WOODEN_WILD_MOUSE }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_WOODEN_WILD_MOUSE)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE)),
    SET_FIELD(Heights, { 14, 24, 4, 7, }),
    SET_FIELD(MaxMass, 4),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftClassic, 4, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_wooden_wild_mouse),
    SET_FIELD(RatingsMultipliers, { 50, 30, 30 }),
    SET_FIELD(UpkeepCosts, { 40, 20, 80, 9, 3, 10 }),
    SET_FIELD(BuildCosts, { 25.00_GBP, 3.00_GBP, 40, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_WILD_WEST),
    SET_FIELD(PhotoItem, ShopItem::Photo3),
    SET_FIELD(BonusValue, 55),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_WHITE },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_SATURATED_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_SATURATED_BROWN },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_WOODEN_WILD_MOUSE_TRACK, SPR_RIDE_DESIGN_PREVIEW_WOODEN_WILD_MOUSE_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
<<<<<<< HEAD
    SET_FIELD(StartRideMusic, OpenRCT2::RideAudio::DefaultStartRideMusicChannel),
    SET_FIELD(Name, "wooden_wild_mouse"),
=======
>>>>>>> 2e1d52f37 (Default to DefaultStartRideMusicChannel)
};
// clang-format on
