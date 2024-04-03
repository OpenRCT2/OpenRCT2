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
constexpr RideTypeDescriptor kSuspendedSwingingCoasterRTD =
{
    .AlternateType = kRideTypeNull,
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_CURVE_LARGE, TRACK_HELIX_DOWN_UNBANKED_QUARTER, TRACK_HELIX_UP_UNBANKED_QUARTER, TRACK_BRAKES, TRACK_BLOCK_BRAKES},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::kEndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionSuspendedSwingingRC),
    .Flags = kRideTypeFagsTrackHas3Colours | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     kRideTypeFlagsCommonCoaster | kRideTypeFlagsCommonCoasterNonAlt |
                     RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES | RIDE_TYPE_FLAG_IS_SUSPENDED,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 0, 0, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_SUSPENDED_SWINGING_COASTER, STR_RIDE_DESCRIPTION_SUSPENDED_SWINGING_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .kEnumName = nameof(RIDE_TYPE_SUSPENDED_SWINGING_COASTER),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 24, 40, 29, 8, },
    .MaxMass = 26,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 20, 3, 10 },
    .BuildCosts = { 47.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo2,
    .BonusValue = 90,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_LIGHT_BROWN },
        { COLOUR_BLACK, COLOUR_LIGHT_BLUE, COLOUR_BLACK },
        { COLOUR_SATURATED_BROWN, COLOUR_YELLOW, COLOUR_SATURATED_BROWN },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_OLIVE_GREEN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_SWINGING_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_SWINGING_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "suspended_swinging_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(3, 30), RIDE_RATING(2, 90), RIDE_RATING(3, 50) },
        18,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 40), RIDE_RATING(0, 10), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                32768, 23831, 79437 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 48036 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6971, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 8,                2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xC0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, FIXED_2DP(0, 60), 2, 2, 2 },
            { RatingsModifierType::RequirementLateralGs,  FIXED_2DP(1, 50), 2, 2, 2 },
            { RatingsModifierType::RequirementLength,     0x1720000,        2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                32768, 23831, 79437 },
        },
    },
};
// clang-format on
