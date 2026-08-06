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
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::inlineTwistUninverted, TrackGroup::flyingHalfLoopUninvertedUp, TrackGroup::quarterLoopUninvertedUp, TrackGroup::flyingLargeHalfLoopUninvertedUp, TrackGroup::slopeVertical, TrackGroup::slopeCurveBanked, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::slopeCurveLarge, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown, TrackGroup::helixUpUnbankedQuarter, TrackGroup::helixDownUnbankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::helixDownBankedQuarter },
        .extraTrackGroups = {TrackGroup::booster, TrackGroup::slopeSteepLong, TrackGroup::diagSlopeSteepLong, TrackGroup::flyingLargeHalfLoopUninvertedDown, TrackGroup::flyingHalfLoopUninvertedDown, TrackGroup::stationEnd, TrackGroup::verticalLoop, TrackGroup::poweredLift},
    }),
    .InvertedTrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::flyingRollerCoasterInverted,
        .supportType = MetalSupportType::tubesInverted,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::inlineTwistInverted, TrackGroup::flyingHalfLoopInvertedDown, TrackGroup::quarterLoopInvertedDown, TrackGroup::flyingLargeHalfLoopInvertedDown, TrackGroup::booster, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown },
        .extraTrackGroups = {TrackGroup::flyingLargeHalfLoopInvertedUp, TrackGroup::flyingHalfLoopInvertedUp},
    }),
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::hasInvertedVariant, 
                     RtdFlag::checkGForces, RtdFlag::allowMultipleCircuits, RtdFlag::startConstructionInverted),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_FLYING_ROLLER_COASTER, STR_RIDE_DESCRIPTION_FLYING_ROLLER_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
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
    .ColourKey = RideColourKey::ride,
    .Name = "flying_rc",
    .RatingsData = 
    {
        RatingsCalculationType::normal,
        { RideRating::make(4, 35), RideRating::make(1, 85), RideRating::make(4, 33) },
        17,
        kDynamicRideShelterRating,
        true,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                24576, 38130, 49648 },
            { RatingsModifierType::bonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::bonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::requirementInversions, 1,                2, 1, 1 },
            { RatingsModifierType::requirementMaxSpeed,   0xA0000,          2, 1, 1 },
            { RatingsModifierType::requirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 1, 1 },
            { RatingsModifierType::requirementNumDrops,   2,                2, 1, 1 },
            { RatingsModifierType::penaltyLateralGs,      0,                24576, 38130, 49648 },
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
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster |
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::startConstructionInverted, RtdFlag::isDummyType),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_3A, STR_RIDE_DESCRIPTION_UNKNOWN },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
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
    .ColourKey = RideColourKey::ride,
    .Name = "flying_rc_alt",
    .RatingsData = 
    {
        RatingsCalculationType::normal,
        { RideRating::make(4, 35), RideRating::make(1, 85), RideRating::make(4, 33) },
        17,
        kDynamicRideShelterRating,
        true,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                24576, 38130, 49648 },
            { RatingsModifierType::bonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::bonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::requirementInversions, 1,                2, 1, 1 },
            { RatingsModifierType::requirementMaxSpeed,   0xA0000,          2, 1, 1 },
            { RatingsModifierType::requirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 1, 1 },
            { RatingsModifierType::requirementNumDrops,   2,                2, 1, 1 },
            { RatingsModifierType::penaltyLateralGs,      0,                24576, 38130, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
