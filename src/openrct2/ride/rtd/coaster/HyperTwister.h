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
#include "../../../paint/support/MetalSupports.h"
#include "../../RideData.h"
#include "../../ShopItem.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor HyperTwisterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::twisterRollerCoaster,
        .supportType = MetalSupportType::tubes,
        .enabledTrackGroups = { TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeCurveBanked, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::slopeSteepLong, TrackGroup::slopeVertical, TrackGroup::curveVertical, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::slopeCurveLarge, TrackGroup::diagSlopeSteepLong, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown, TrackGroup::helixUpUnbankedQuarter, TrackGroup::helixDownUnbankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::helixDownBankedQuarter },
        .extraTrackGroups = { TrackGroup::liftHillSteep, TrackGroup::brakeForDrop, TrackGroup::booster, TrackGroup::poweredLift, TrackGroup::verticalLoop, TrackGroup::halfLoop, TrackGroup::corkscrew, TrackGroup::barrelRoll, TrackGroup::halfLoopLarge, TrackGroup::quarterLoop, TrackGroup::twist, TrackGroup::corkscrewLarge, TrackGroup::halfLoopMedium, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::diveLoop },
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, 
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .TrackSpeedSettings = { 60, 60 },
    .BoosterSettings = { 17, 68 },
    .LegacyBoosterSettings = { 17, 68 },
    .Naming = { STR_RIDE_NAME_HYPER_TWISTER, STR_RIDE_DESCRIPTION_HYPER_TWISTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station},
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 61, 24, 8, 9, },
    .MaxMass = 31,
    .LiftData = { Audio::SoundId::liftBM, 5, 8 },
    .RatingsMultipliers = { 52, 36, 10 },
    .UpkeepCosts = { 43, 20, 80, 11, 3, 10 },
    .BuildCosts = { 60.00_GBP, 2.50_GBP, 55, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 120,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::yellow, Drawing::Colour::yellow, Drawing::Colour::bordeauxRed },
        { Drawing::Colour::lightWater, Drawing::Colour::lightWater, Drawing::Colour::darkPurple },
        { Drawing::Colour::white, Drawing::Colour::white, Drawing::Colour::lightBlue },
        { Drawing::Colour::darkGreen, Drawing::Colour::mossGreen, Drawing::Colour::darkBrown },
        { Drawing::Colour::bordeauxRed, Drawing::Colour::lightOrange, Drawing::Colour::white },
        { Drawing::Colour::darkPurple, Drawing::Colour::violet, Drawing::Colour::deepWater }, // Mako
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_TWISTER_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_TWISTER_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "hyper_twister",
    .RatingsData = 
    {
        RatingsCalculationType::normal,
        { RideRating::make(3, 50), RideRating::make(0, 40), RideRating::make(0, 30) },
        15,
        kDynamicRideShelterRating,
        true,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                24576, 32768, 49648 },
            { RatingsModifierType::bonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::bonusReversedTrains,   0,                2, 20, 30 },
            { RatingsModifierType::bonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 12,               2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::requirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 2, 2 },
            { RatingsModifierType::requirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                24576, 32768, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
