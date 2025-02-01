/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
        .trackStyle = TrackStyle::multiDimensionRollerCoaster,
        .supportType = MetalSupportType::TubesInverted,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::blockBrakes, TrackGroup::inlineTwistUninverted,TrackGroup::quarterLoopUninvertedUp, TrackGroup::quarterLoopUninvertedDown, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::multiDimensionRollerCoasterInverted,
        .supportType = MetalSupportType::TubesInverted,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::blockBrakes, TrackGroup::inlineTwistInverted, TrackGroup::quarterLoopInvertedUp, TrackGroup::quarterLoopInvertedDown, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes},
        .extraTrackGroups = {},
    }),
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::hasInvertedVariant, 
                     RtdFlag::checkGForces, RtdFlag::allowMultipleCircuits, RtdFlag::hasSeatRotation,
                     RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_MULTI_DIMENSION_ROLLER_COASTER, STR_RIDE_DESCRIPTION_MULTI_DIMENSION_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 40, 24, 8, 11, },
    .MaxMass = 78,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftFrictionWheels, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 75, 20, 90, 11, 3, 15 },
    .BuildCosts = { 90.00_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock3,
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
        { MakeRideRating(3, 75), MakeRideRating(1, 95), MakeRideRating(4, 79) },
        18,
        -1,
        true,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 05), 0 },
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
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 1, 1 },
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
        .trackStyle = TrackStyle::null,
        .enabledTrackGroups = {  },
        .extraTrackGroups = {  },
    }),
    .InvertedTrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::null,
        .enabledTrackGroups = {  },
        .extraTrackGroups = {  },
    }),
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | 
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::hasSeatRotation),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_38, STR_RIDE_DESCRIPTION_UNKNOWN },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 40, 24, 8, 11, },
    .MaxMass = 78,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftFrictionWheels, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 75, 20, 90, 11, 3, 15 },
    .BuildCosts = { 90.00_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock3,
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
        { MakeRideRating(3, 75), MakeRideRating(1, 95), MakeRideRating(4, 79) },
        18,
        -1,
        true,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 05), 0 },
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
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 1, 1 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 38130, 49648 },
        },
    },
};

// clang-format on
