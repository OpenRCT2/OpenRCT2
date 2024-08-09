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
constexpr RideTypeDescriptor MultiDimensionRollerCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionMultiDimensionRC,
        .EnabledTrackPieces = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::blockBrakes, TrackGroup::inlineTwistUninverted,TrackGroup::quarterLoopUninvertedUp, TrackGroup::quarterLoopUninvertedDown, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionMultiDimensionRC,
        .EnabledTrackPieces = {TrackGroup::straight, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::blockBrakes, TrackGroup::inlineTwistInverted, TrackGroup::quarterLoopInvertedUp, TrackGroup::quarterLoopInvertedDown},
        .ExtraTrackPieces = {},
    }),
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT |
                     RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES |
                     RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS | RIDE_TYPE_FLAG_HAS_SEAT_ROTATION |
                     RIDE_TYPE_FLAG_ALLOW_REVERSED_TRAINS,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_MULTI_DIMENSION_ROLLER_COASTER, STR_RIDE_DESCRIPTION_MULTI_DIMENSION_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 40, 24, 8, 11, },
    .MaxMass = 78,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftFrictionWheels, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 75, 20, 90, 11, 3, 15 },
    .BuildCosts = { 90.00_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_ROCK_3,
    .PhotoItem = ShopItem::Photo2,
    .BonusValue = 100,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_PINK, COLOUR_YELLOW, COLOUR_YELLOW }, // X
        { COLOUR_LIGHT_PURPLE, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED },
        { COLOUR_BORDEAUX_RED, COLOUR_WHITE, COLOUR_WHITE },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BLACK }, // X2
        { COLOUR_BORDEAUX_RED_DARK, COLOUR_DARK_YELLOW, COLOUR_OLIVE_DARK }, // Dinoconda
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_GREY }, // Eejanaika
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MULTI_DIMENSION_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_MULTI_DIMENSION_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "multi_dimension_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(3, 75), RIDE_RATING(1, 95), RIDE_RATING(4, 79) },
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
            { RatingsModifierType::RequirementInversions, 1,                4, 1, 1 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 1, 1 },
            { RatingsModifierType::RequirementNegativeGs, FIXED_2DP(0, 40), 2, 1, 1 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 38130, 49648 },
        },
    },
};

constexpr RideTypeDescriptor MultiDimensionRollerCoasterAltRTD =
{
    .Category = RIDE_CATEGORY_NONE,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
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
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAG_HAS_SEAT_ROTATION,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_38, STR_RIDE_DESCRIPTION_UNKNOWN },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 40, 24, 8, 11, },
    .MaxMass = 78,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftFrictionWheels, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 75, 20, 90, 11, 3, 15 },
    .BuildCosts = { 90.00_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_ROCK_3,
    .PhotoItem = ShopItem::Photo2,
    .BonusValue = 100,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_PINK, COLOUR_YELLOW, COLOUR_YELLOW },
        { COLOUR_LIGHT_PURPLE, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED },
        { COLOUR_BORDEAUX_RED, COLOUR_WHITE, COLOUR_WHITE },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MULTI_DIMENSION_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_MULTI_DIMENSION_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "multi_dimension_rc_alt",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(3, 75), RIDE_RATING(1, 95), RIDE_RATING(4, 79) },
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
            { RatingsModifierType::RequirementInversions, 1,                4, 1, 1 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 1, 1 },
            { RatingsModifierType::RequirementNegativeGs, FIXED_2DP(0, 40), 2, 1, 1 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 38130, 49648 },
        },
    },
};

// clang-format on
