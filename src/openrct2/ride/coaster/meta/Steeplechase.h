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
constexpr const RideTypeDescriptor SteeplechaseRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_SLOPE, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_BRAKES, TRACK_BLOCK_BRAKES}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionSteeplechase),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_HAS_LARGE_CURVES |
                     RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_STEEPLECHASE, STR_RIDE_DESCRIPTION_STEEPLECHASE }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_STEEPLECHASE)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE)),
    SET_FIELD(Heights, { 14, 24, 7, 7, }),
    SET_FIELD(MaxMass, 4),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftClassic, 4, 5 }),
    SET_FIELD(RatingsCalculationFunction, RideRatingsCalculateSteeplechase),
    SET_FIELD(RatingsMultipliers, { 50, 30, 10 }),
    SET_FIELD(UpkeepCosts, { 40, 20, 80, 10, 3, 10 }),
    SET_FIELD(BuildCosts, { 28.50_GBP, 2.00_GBP, 40, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_1),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 60),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_BLACK },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BLACK },
        { COLOUR_BRIGHT_GREEN, COLOUR_BRIGHT_GREEN, COLOUR_BLACK },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_LIGHT_ORANGE, COLOUR_LIGHT_ORANGE, COLOUR_BLACK },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_BLACK },
        { COLOUR_BRIGHT_PINK, COLOUR_BRIGHT_PINK, COLOUR_BLACK },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_STEEPLECHASE_TRACK, SPR_RIDE_DESIGN_PREVIEW_STEEPLECHASE_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "steeplechase"),
};
// clang-format on
