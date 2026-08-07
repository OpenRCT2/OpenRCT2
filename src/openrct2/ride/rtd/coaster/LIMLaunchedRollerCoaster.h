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
constexpr RideTypeDescriptor LIMLaunchedRollerCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::limLaunchedRollerCoaster,
        .supportType = MetalSupportType::tubes,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::flatRollBanking, TrackGroup::verticalLoop, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::twist, TrackGroup::corkscrew, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::curveVertical, TrackGroup::quarterLoop,  TrackGroup::barrelRoll, TrackGroup::slopeCurveBanked, TrackGroup::slopeSteepLong, TrackGroup::halfLoop, TrackGroup::halfLoopMedium, TrackGroup::halfLoopLarge, TrackGroup::corkscrewLarge, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown, TrackGroup::helixUpUnbankedQuarter, TrackGroup::helixDownUnbankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::helixDownBankedQuarter },
        .extraTrackGroups = {TrackGroup::flatToSteepSlope},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::poweredLaunchPassthrough, RideMode::poweredLaunch, RideMode::poweredLaunchBlockSectioned),
    .DefaultMode = RideMode::poweredLaunch,
    .OperatingSettings = { 10, 31 },
    .BoosterSettings = { 18, 52 },
    .LegacyBoosterSettings = { 18, 52 },
    .Naming = { STR_RIDE_NAME_LIM_LAUNCHED_ROLLER_COASTER, STR_RIDE_DESCRIPTION_LIM_LAUNCHED_ROLLER_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 35, 24, 5, 7, },
    .MaxMass = 18,
    .LiftData = { Audio::SoundId::null, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 42, 20, 80, 9, 3, 10 },
    .BuildCosts = { 47.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::brightGreen, Drawing::Colour::brightGreen, Drawing::Colour::white },
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::darkBrown },
        { Drawing::Colour::yellow, Drawing::Colour::yellow, Drawing::Colour::saturatedGreen },
        { Drawing::Colour::brightGreen, Drawing::Colour::limeGreen, Drawing::Colour::violet}, // Joker's Jinx
        { Drawing::Colour::limeGreen, Drawing::Colour::chartreuse , Drawing::Colour::deepWater}, // Poltergeist
        { Drawing::Colour::grey, Drawing::Colour::darkBrown , Drawing::Colour::black}, // Flight of Fear
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_LIM_LAUNCHED_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_LIM_LAUNCHED_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "lim_launched_rc",
    .RatingsData = 
    {
        RatingsCalculationType::normal,
        { RideRating::make(2, 90), RideRating::make(1, 50), RideRating::make(2, 20) },
        25,
        kDynamicRideShelterRating,
        true,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                24576, 35746, 49648 },
            { RatingsModifierType::bonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::bonusReversedTrains,   0,                2, 15, 20 },
            { RatingsModifierType::bonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 10,               2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::requirementNegativeGs, 10,               2, 2, 2 },
            { RatingsModifierType::requirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                24576, 35746, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
