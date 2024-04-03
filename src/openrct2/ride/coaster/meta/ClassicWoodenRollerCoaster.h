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
constexpr RideTypeDescriptor kClassicWoodenRollerCoasterRTD =
{
   .AlternateType = kRideTypeNull,
   .Category = RIDE_CATEGORY_ROLLERCOASTER,
   .EnabledTrackPieces = {TRACK_FLAT, TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_FLAT_ROLL_BANKING, TRACK_VERTICAL_LOOP, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_CURVE_LARGE, TRACK_BRAKES, TRACK_ON_RIDE_PHOTO, TRACK_WATER_SPLASH, TRACK_BLOCK_BRAKES, TRACK_DIAG_BRAKES, TRACK_DIAG_BLOCK_BRAKES},
   .ExtraTrackPieces = {},
   .CoveredTrackPieces = 0,
   .StartTrackPiece = TrackElemType::kEndStation,
   .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionClassicWoodenRC),
   .Flags = kRideTypeFagsTrackHas3Colours | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                    kRideTypeFlagsCommonCoaster | kRideTypeFlagsCommonCoasterNonAlt |
                    RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES | RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS |
                    RIDE_TYPE_FLAG_ALLOW_REVERSED_TRAINS,
   .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
   .DefaultMode = RideMode::ContinuousCircuit,
   .OperatingSettings = { 0, 0, 0, 0, 68, 0 },
   .Naming = { STR_RIDE_NAME_CLASSIC_WOODEN_ROLLER_COASTER, STR_RIDE_DESCRIPTION_CLASSIC_WOODEN_ROLLER_COASTER },
   .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
   .kEnumName = nameof(RIDE_TYPE_CLASSIC_WOODEN_ROLLER_COASTER),
   .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
   .Heights = { 24, 24, 8, 11, },
   .MaxMass = 19,
   .LiftData = { OpenRCT2::Audio::SoundId::LiftWood, 3, 5 },
   .RatingsMultipliers = { 52, 33, 4 },
   .UpkeepCosts = { 40, 20, 80, 10, 3, 10 },
   .BuildCosts = {37.50_GBP, 3.50_GBP, 50,  },
   .DefaultPrices = { 20, 20 },
   .DefaultMusic = kMusicObjectWildWest,
   .PhotoItem = ShopItem::Photo3,
   .BonusValue = 105,
   .ColourPresets = TRACK_COLOUR_PRESETS(
       { COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN},
       { COLOUR_WHITE, COLOUR_LIGHT_BROWN, COLOUR_WHITE },
       { COLOUR_LIGHT_BROWN, COLOUR_BLACK, COLOUR_LIGHT_BROWN },
   ),
   .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_CLASSIC_WOODEN_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_CLASSIC_WOODEN_ROLLER_COASTER_SUPPORTS },
   .ColourKey = RideColourKey::Ride,
   .Name = "classic_wooden_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 80), RIDE_RATING(2, 60), RIDE_RATING(2, 00) },
        19,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             873, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 40), RIDE_RATING(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                364088, 655360, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                40960, 34555, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 43458, 45749 },
            { RatingsModifierType::BonusDrops,            0,                40777, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 12, 22 },
            { RatingsModifierType::BonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 12,               2, 1, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 1, 2 },
            { RatingsModifierType::RequirementNegativeGs, FIXED_2DP(0, 10), 2, 1, 2 },
            { RatingsModifierType::RequirementLength,     0x1720000,        2, 1, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 1, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                40960, 34555, 49648 },
        },
    },
};
// clang-format on
