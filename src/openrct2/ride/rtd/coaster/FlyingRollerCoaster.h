/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../SpriteIds.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
// Non-inverted variant
constexpr RideTypeDescriptor FlyingRollerCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::flyingRollerCoaster,
        .supportType = MetalSupportType::tubesInverted,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::inlineTwistUninverted, TrackGroup::flyingHalfLoopUninvertedUp, TrackGroup::quarterLoopUninvertedUp, TrackGroup::flyingLargeHalfLoopUninvertedUp, TrackGroup::slopeVertical, TrackGroup::slopeCurveBanked, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::slopeCurveLarge, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown },
        .extraTrackGroups = {TrackGroup::booster, TrackGroup::slopeSteepLong, TrackGroup::diagSlopeSteepLong, TrackGroup::flyingLargeHalfLoopUninvertedDown, TrackGroup::flyingHalfLoopUninvertedDown, TrackGroup::stationEnd, TrackGroup::verticalLoop, TrackGroup::poweredLift},
    }),
    .InvertedTrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::flyingRollerCoasterInverted,
        .supportType = MetalSupportType::tubesInverted,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::inlineTwistInverted, TrackGroup::flyingHalfLoopInvertedDown, TrackGroup::quarterLoopInvertedDown, TrackGroup::flyingLargeHalfLoopInvertedDown, TrackGroup::booster, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown },
        .extraTrackGroups = {TrackGroup::flyingLargeHalfLoopInvertedUp, TrackGroup::flyingHalfLoopInvertedUp},
    }),
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::hasInvertedVariant, 
                     RtdFlag::checkGForces, RtdFlag::allowMultipleCircuits, RtdFlag::startConstructionInverted),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_FLYING_ROLLER_COASTER, STR_RIDE_DESCRIPTION_FLYING_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 30, 24, 8, 11, },
    .MaxMass = 35,
    .LiftData = { Audio::SoundId::liftBM, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 49, 20, 90, 11, 3, 15 },
    .BuildCosts = { 62.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo2,
    .BonusValue = 100,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::oliveGreen, Drawing::Colour::oliveGreen, Drawing::Colour::darkWater },
        { Drawing::Colour::darkWater, Drawing::Colour::darkWater, Drawing::Colour::bordeauxRed },
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::darkPurple },
        { Drawing::Colour::viridian, Drawing::Colour::limeGreen, Drawing::Colour::deepWater },
        { Drawing::Colour::maroon, Drawing::Colour::maroon, Drawing::Colour::saturatedBrown }, // FLY
        { Drawing::Colour::lightBlue, Drawing::Colour::darkBlue, Drawing::Colour::icyBlue }, // Manta
        { Drawing::Colour::yellow, Drawing::Colour::brightRed, Drawing::Colour::lightOrange } // Tatsu
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "flying_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(4, 35), RideRating::make(1, 85), RideRating::make(4, 33) },
        17,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
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
            { RatingsModifierType::RequirementInversions, 1,                2, 1, 1 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 1, 1 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 1, 1 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 38130, 49648 },
        },
    },
};

constexpr RideTypeDescriptor FlyingRollerCoasterAltRTD =
{
    .Category = RideCategory::none,
    .StartTrackPiece = TrackElemType::endStation,
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
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::startConstructionInverted),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_3A, STR_RIDE_DESCRIPTION_UNKNOWN },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 30, 24, 8, 11, },
    .MaxMass = 35,
    .LiftData = { Audio::SoundId::liftBM, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 49, 20, 90, 11, 3, 15 },
    .BuildCosts = { 62.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo2,
    .BonusValue = 100,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::oliveGreen, Drawing::Colour::oliveGreen, Drawing::Colour::darkWater },
        { Drawing::Colour::darkWater, Drawing::Colour::darkWater, Drawing::Colour::bordeauxRed },
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::darkPurple },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "flying_rc_alt",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(4, 35), RideRating::make(1, 85), RideRating::make(4, 33) },
        17,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
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
            { RatingsModifierType::RequirementInversions, 1,                2, 1, 1 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 1, 1 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 1, 1 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 38130, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
