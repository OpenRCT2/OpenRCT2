/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
// Non-inverted variant
constexpr const RideTypeDescriptor FlyingRollerCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_FLYING_ROLLER_COASTER_ALT),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_FLAT_ROLL_BANKING, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE, TRACK_SLOPE_CURVE_STEEP, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_HELIX_SMALL, TRACK_BRAKES, TRACK_ON_RIDE_PHOTO, TRACK_BLOCK_BRAKES, TRACK_SLOPE_ROLL_BANKING, TRACK_INLINE_TWIST_UNINVERTED, TRACK_FLYING_HALF_LOOP_UNINVERTED_UP, TRACK_QUARTER_LOOP_UNINVERTED_UP, TRACK_FLYING_LARGE_HALF_LOOP_UNINVERTED_UP, TRACK_SLOPE_VERTICAL, TRACK_SLOPE_CURVE_BANKED}),
    SET_FIELD(ExtraTrackPieces, {TRACK_BOOSTER, TRACK_SLOPE_STEEP_LONG, TRACK_FLYING_LARGE_HALF_LOOP_UNINVERTED_DOWN, TRACK_FLYING_HALF_LOOP_UNINVERTED_DOWN, TRACK_STATION_END, TRACK_VERTICAL_LOOP, TRACK_POWERED_LIFT}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionFlyingRC),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_HAS_LARGE_CURVES |
                     RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES |
                     RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS | RIDE_TYPE_FLAG_START_CONSTRUCTION_INVERTED),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 10, 27, 30, 25, 25, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_FLYING_ROLLER_COASTER, STR_RIDE_DESCRIPTION_FLYING_ROLLER_COASTER }),
    SET_FIELD(NameConvention, { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_FLYING_ROLLER_COASTER)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE)),
    SET_FIELD(Heights, { 30, 24, 8, 11, }),
    SET_FIELD(MaxMass, 35),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftBM, 4, 6 }),
    SET_FIELD(RatingsCalculationFunction, RideRatingsCalculateFlyingRollerCoaster),
    SET_FIELD(RatingsMultipliers, { 50, 30, 10 }),
    SET_FIELD(UpkeepCosts, { 49, 20, 90, 11, 3, 15 }),
    SET_FIELD(BuildCosts, { 62.50_GBP, 2.50_GBP, 50, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_1),
    SET_FIELD(PhotoItem, ShopItem::Photo2),
    SET_FIELD(BonusValue, 100),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_TEAL },
        { COLOUR_TEAL, COLOUR_TEAL, COLOUR_BORDEAUX_RED },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_DARK_PURPLE },
        { COLOUR_BORDEAUX_RED_DARK, COLOUR_BORDEAUX_RED_DARK, COLOUR_SATURATED_BROWN }, // FLY
        { COLOUR_LIGHT_BLUE, COLOUR_DARK_BLUE, COLOUR_ICY_BLUE }, // Manta
        { COLOUR_YELLOW, COLOUR_BRIGHT_RED, COLOUR_LIGHT_ORANGE } // Tatsu
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "flying_rc"),
};

// Inverted variant
constexpr const RideTypeDescriptor FlyingRollerCoasterAltRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_NONE),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_FLAT_ROLL_BANKING, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE, TRACK_SLOPE_CURVE_STEEP, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_HELIX_LARGE, TRACK_BRAKES, TRACK_ON_RIDE_PHOTO, TRACK_BLOCK_BRAKES, TRACK_SLOPE_ROLL_BANKING, TRACK_INLINE_TWIST_INVERTED, TRACK_FLYING_HALF_LOOP_INVERTED_DOWN, TRACK_QUARTER_LOOP_INVERTED_DOWN, TRACK_FLYING_LARGE_HALF_LOOP_INVERTED_DOWN, TRACK_BOOSTER}),
    SET_FIELD(ExtraTrackPieces, {TRACK_FLYING_LARGE_HALF_LOOP_INVERTED_UP, TRACK_FLYING_HALF_LOOP_INVERTED_UP}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, nullptr),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAG_HAS_LARGE_CURVES |
                     RIDE_TYPE_FLAG_START_CONSTRUCTION_INVERTED),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 10, 27, 30, 25, 25, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_3A, STR_RIDE_DESCRIPTION_UNKNOWN }),
    SET_FIELD(NameConvention, { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_FLYING_ROLLER_COASTER_ALT)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE)),
    SET_FIELD(Heights, { 30, 24, 8, 11, }),
    SET_FIELD(MaxMass, 35),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftBM, 4, 6 }),
    SET_FIELD(RatingsCalculationFunction, RideRatingsCalculateFlyingRollerCoaster),
    SET_FIELD(RatingsMultipliers, { 50, 30, 10 }),
    SET_FIELD(UpkeepCosts, { 49, 20, 90, 11, 3, 15 }),
    SET_FIELD(BuildCosts, { 62.50_GBP, 2.50_GBP, 50, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_1),
    SET_FIELD(PhotoItem, ShopItem::Photo2),
    SET_FIELD(BonusValue, 100),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_TEAL },
        { COLOUR_TEAL, COLOUR_TEAL, COLOUR_BORDEAUX_RED },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_DARK_PURPLE },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "flying_rc_alt"),
};
// clang-format on
