/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
constexpr RideTypeDescriptor WoodenWildMouseRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionWoodenWildMouse,
        .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_LIFT_HILL_STEEP, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_LONG, TRACK_CURVE_VERY_SMALL, TRACK_CURVE_SMALL},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT |
                     RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION | RIDE_TYPE_FLAG_ALLOW_DOORS_ON_TRACK |
                     RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_WOODEN_WILD_MOUSE, STR_RIDE_DESCRIPTION_WOODEN_WILD_MOUSE },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_WOODEN_WILD_MOUSE",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 14, 24, 4, 7, },
    .MaxMass = 4,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 4, 5 },
    .RatingsMultipliers = { 50, 30, 30 },
    .UpkeepCosts = { 40, 20, 80, 9, 3, 10 },
    .BuildCosts = { 25.00_GBP, 3.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_WILD_WEST,
    .PhotoItem = ShopItem::Photo3,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_WHITE },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_SATURATED_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_SATURATED_BROWN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_WOODEN_WILD_MOUSE_TRACK, SPR_RIDE_DESIGN_PREVIEW_WOODEN_WILD_MOUSE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "wooden_wild_mouse",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 90), RIDE_RATING(2, 90), RIDE_RATING(2, 10) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             873, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 40), RIDE_RATING(0, 8), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                364088, 655360, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                102400, 35746, 49648 },
            { RatingsModifierType::BonusTurns,            0,                29721, 43458, 45749 },
            { RatingsModifierType::BonusDrops,            0,                40777, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::BonusProximity,        0,                17893, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                5577, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 8,                2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0x70000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, FIXED_2DP(0, 10), 2, 2, 2 },
            { RatingsModifierType::RequirementLateralGs,  FIXED_2DP(1, 50), 2, 2, 2 },
            { RatingsModifierType::RequirementLength,     0xAA0000,         2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   3,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                102400, 35746, 49648 },
        },
    },
};
// clang-format on
