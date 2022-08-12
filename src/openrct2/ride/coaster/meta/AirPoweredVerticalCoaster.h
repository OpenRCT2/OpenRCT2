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
constexpr const RideTypeDescriptor AirPoweredVerticalCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL_STEEP, TRACK_LIFT_HILL_CURVE, TRACK_FLAT_ROLL_BANKING, TRACK_CURVE, TRACK_BRAKES, TRACK_REVERSE_FREEFALL, TRACK_SLOPE_TO_FLAT, TRACK_ON_RIDE_PHOTO}),
    SET_FIELD(ExtraTrackPieces, {TRACK_BOOSTER}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_air_powered_vertical_rc),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::PoweredLaunchPasstrough, RideMode::PoweredLaunch)),
    SET_FIELD(DefaultMode, RideMode::PoweredLaunchPasstrough),
    SET_FIELD(OperatingSettings, { 30, 50, 30, 40, 40, 0, 13 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_AIR_POWERED_VERTICAL_COASTER, STR_RIDE_DESCRIPTION_AIR_POWERED_VERTICAL_COASTER }),
    SET_FIELD(NameConvention, { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 255, 32, 4, 7, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_air_powered_vertical_coaster),
    SET_FIELD(RatingsMultipliers, { 44, 66, 10 }),
    SET_FIELD(UpkeepCosts, { 90, 20, 0, 0, 0, 10 }),
    SET_FIELD(BuildCosts, { 100.00_GBP, 0.00_GBP, 45, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_2),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 70),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_BRIGHT_RED, COLOUR_DARK_BROWN },
        { COLOUR_BRIGHT_RED, COLOUR_LIGHT_BLUE, COLOUR_WHITE },
        { COLOUR_LIGHT_ORANGE, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_YELLOW },
        { COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_TEAL },
        { COLOUR_TEAL, COLOUR_TEAL, COLOUR_BORDEAUX_RED },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_DARK_PURPLE },
        { COLOUR_SATURATED_RED, COLOUR_SATURATED_RED, COLOUR_DARK_YELLOW },
        { COLOUR_LIGHT_BLUE, COLOUR_DARK_BLUE, COLOUR_ICY_BLUE },
        { COLOUR_YELLOW, COLOUR_BRIGHT_RED, COLOUR_LIGHT_ORANGE },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_AIR_POWERED_VERTICAL_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_AIR_POWERED_VERTICAL_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "air_powered_vertical_rc"),
};
// clang-format on
