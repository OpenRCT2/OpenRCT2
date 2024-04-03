/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
constexpr RideTypeDescriptor kSpinningWildMouseRTD =
{
    .AlternateType = kRideTypeNull,
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .EnabledTrackPieces = {TRACK_STRAIGHT,TRACK_STATION_END,TRACK_LIFT_HILL,  TRACK_SLOPE,TRACK_SLOPE_LONG,TRACK_CURVE_VERY_SMALL,TRACK_CURVE_SMALL, TRACK_BRAKES,TRACK_BLOCK_BRAKES,TRACK_ROTATION_CONTROL_TOGGLE},
    .ExtraTrackPieces = {TRACK_LIFT_HILL_STEEP, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::kEndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionWildMouse),
    .Flags = kRideTypeFagsTrackHas3Colours | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     kRideTypeFlagsCommonCoaster | kRideTypeFlagsCommonCoasterNonAlt | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 0, 0, 0, 0, 0, 0 },
    .Naming = { STR_SPINNING_WILD_MOUSE_GROUP, STR_SPINNING_WILD_MOUSE_GROUP_DESC },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .kEnumName = nameof(RIDE_TYPE_SPINNING_WILD_MOUSE),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 16, 24, 4, 7, },
    .MaxMass = 4,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftWildMouse, 4, 6 },
    .RatingsMultipliers = { 50, 30, 30 },
    .UpkeepCosts = { 40, 20, 80, 9, 3, 10 },
    .BuildCosts = { 27.50_GBP, 2.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_YELLOW },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_WHITE },
        { COLOUR_DARK_PURPLE, COLOUR_BRIGHT_PINK, COLOUR_DARK_PURPLE },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_BORDEAUX_RED },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_WILD_MOUSE_TRACK, SPR_RIDE_DESIGN_PREVIEW_WILD_MOUSE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "spinning_wild_mouse",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 80), RIDE_RATING(2, 50), RIDE_RATING(2, 10) },
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
            { RatingsModifierType::RequirementDropHeight, 6,                2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0x70000,          2, 2, 2 },
            { RatingsModifierType::RequirementLateralGs,  FIXED_2DP(1, 50), 2, 2, 2 },
            { RatingsModifierType::RequirementLength,     0xAA0000,         2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                102400, 35746, 49648 },
        },
    },
};
// clang-format on
