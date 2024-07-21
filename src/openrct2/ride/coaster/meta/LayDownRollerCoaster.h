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
#include "../CorkscrewRollerCoaster.hpp"

// clang-format off
constexpr RideTypeDescriptor LayDownRollerCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(
        { 
            .Drawer = GetTrackPaintFunctionCorkscrewRC<MetalSupportType::TubesInverted>,
            .EnabledTrackPieces = { TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_FLAT_ROLL_BANKING, TRACK_VERTICAL_LOOP, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE, TRACK_SLOPE_CURVE_STEEP, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_CURVE_LARGE, TRACK_HELIX_DOWN_BANKED_HALF, TRACK_HELIX_UP_BANKED_HALF, TRACK_BRAKES, TRACK_ON_RIDE_PHOTO, TRACK_BLOCK_BRAKES, TRACK_INLINE_TWIST_UNINVERTED, TRACK_FLYING_HALF_LOOP_UNINVERTED_UP, TRACK_CORKSCREW_UNINVERTED, TRACK_SLOPE_STEEP_LONG, TRACK_DIAG_BRAKES, TRACK_DIAG_BLOCK_BRAKES },
            .ExtraTrackPieces = { TRACK_FLYING_HALF_LOOP_UNINVERTED_DOWN, TRACK_BOOSTER },
        }
    ),
    .InvertedTrackPaintFunctions = TrackDrawerDescriptor(
        {
            .Drawer = GetTrackPaintFunctionLayDownRCInverted,
            .EnabledTrackPieces = { TRACK_STRAIGHT, TRACK_FLAT_ROLL_BANKING, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE, TRACK_SLOPE_CURVE_STEEP, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_CURVE_LARGE, TRACK_HELIX_DOWN_BANKED_QUARTER, TRACK_HELIX_UP_BANKED_QUARTER, TRACK_BRAKES, TRACK_ON_RIDE_PHOTO, TRACK_BLOCK_BRAKES, TRACK_INLINE_TWIST_INVERTED, TRACK_FLYING_HALF_LOOP_INVERTED_DOWN, TRACK_CORKSCREW_INVERTED },
            .ExtraTrackPieces = { TRACK_FLYING_HALF_LOOP_INVERTED_UP, TRACK_STATION_END, TRACK_LIFT_HILL },
        }
    ),
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT |
                     RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES | RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_LAY_DOWN_ROLLER_COASTER, STR_RIDE_DESCRIPTION_LAY_DOWN_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_LAY_DOWN_ROLLER_COASTER",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 26, 24, 8, 11, },
    .MaxMass = 25,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 49, 20, 90, 11, 3, 15 },
    .BuildCosts = { 53.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_ROCK_1,
    .PhotoItem = ShopItem::Photo2,
    .BonusValue = 100,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_YELLOW },
        { COLOUR_DARK_PURPLE, COLOUR_DARK_PURPLE, COLOUR_YELLOW },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_DARK_GREEN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_LAY_DOWN_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_LAY_DOWN_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "lay_down_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(3, 85), RIDE_RATING(1, 15), RIDE_RATING(2, 75) },
        18,
        -1,
        true,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 40), RIDE_RATING(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                24576, 38130, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::RequirementInversions, 1,                4, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, FIXED_2DP(0, 40), 2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 38130, 49648 },
        },
    },
};

constexpr RideTypeDescriptor LayDownRollerCoasterAltRTD =
{
    .Category = RIDE_CATEGORY_NONE,
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({ 
        .Drawer = nullptr,
        .EnabledTrackPieces = {  },
        .ExtraTrackPieces = {  },
    }),
    .InvertedTrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = nullptr,
        .EnabledTrackPieces = {  },
        .ExtraTrackPieces = {  },
    }),
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_40, STR_RIDE_DESCRIPTION_UNKNOWN },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 26, 24, 8, 11, },
    .MaxMass = 25,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 49, 20, 90, 11, 3, 15 },
    .BuildCosts = { 53.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_ROCK_1,
    .PhotoItem = ShopItem::Photo2,
    .BonusValue = 100,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_YELLOW },
        { COLOUR_DARK_PURPLE, COLOUR_DARK_PURPLE, COLOUR_YELLOW },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_DARK_GREEN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_LAY_DOWN_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_LAY_DOWN_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "lay_down_rc_alt",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(3, 85), RIDE_RATING(1, 15), RIDE_RATING(2, 75) },
        18,
        -1,
        true,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 40), RIDE_RATING(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                24576, 38130, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::RequirementInversions, 1,                4, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, FIXED_2DP(0, 40), 2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 38130, 49648 },
        },
    },
};
// clang-format on
