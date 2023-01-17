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
constexpr const RideTypeDescriptor SpiralRollerCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_FLAT_ROLL_BANKING, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE, TRACK_SLOPE_CURVE_STEEP, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_HELIX_SMALL, TRACK_BRAKES, TRACK_ON_RIDE_PHOTO, TRACK_BLOCK_BRAKES, TRACK_SLOPE_ROLL_BANKING, TRACK_LIFT_HILL_CURVED}),
    SET_FIELD(ExtraTrackPieces, {TRACK_BOOSTER, TRACK_LIFT_HILL}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionMiniRc),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_HAS_LARGE_CURVES |
                     RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES | RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 10, 27, 30, 17, 17, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_SPIRAL_ROLLER_COASTER, STR_RIDE_DESCRIPTION_SPIRAL_ROLLER_COASTER }),
    SET_FIELD(NameConvention, { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_SPIRAL_ROLLER_COASTER)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE)),
    SET_FIELD(Heights, { 19, 24, 9, 11, }),
    SET_FIELD(MaxMass, 31),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftFrictionWheels, 7, 7 }),
    SET_FIELD(RatingsCalculationFunction, RideRatingsCalculateSpiralRollerCoaster),
    SET_FIELD(RatingsMultipliers, { 50, 30, 10 }),
    SET_FIELD(UpkeepCosts, { 41, 20, 80, 10, 3, 10 }),
    SET_FIELD(BuildCosts, { 35.00_GBP, 2.50_GBP, 50, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_3),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 85),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_DARK_GREEN },
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BORDEAUX_RED },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_SPIRAL_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_SPIRAL_ROLLER_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "spiral_rc"),
};
// clang-format on
