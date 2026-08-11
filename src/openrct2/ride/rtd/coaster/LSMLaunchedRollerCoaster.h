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
#include "../../RideStringIds.h"
#include "../../ShopItem.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor kLSMLaunchedRollerCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::latticeTriangleAlt,
        .supportType = MetalSupportType::tubes,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::onridePhoto, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::slopeSteepLong, TrackGroup::slopeCurveBanked, TrackGroup::slopeCurveLarge, TrackGroup::slopeCurveLargeBanked, TrackGroup::booster, TrackGroup::diagBooster, TrackGroup::poweredLift, TrackGroup::slopeVertical, TrackGroup::curveVertical, TrackGroup::barrelRoll, TrackGroup::quarterLoop, TrackGroup::halfLoop, TrackGroup::halfLoopMedium, TrackGroup::halfLoopLarge, TrackGroup::verticalLoop, TrackGroup::corkscrew, TrackGroup::corkscrewLarge, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::diagSlopeSteepLong, TrackGroup::diveLoop, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown, TrackGroup::helixUpUnbankedQuarter, TrackGroup::helixDownUnbankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::helixDownBankedQuarter },
        .extraTrackGroups = {TrackGroup::liftHill, TrackGroup::liftHillCable},
        }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
                 RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                              RtdFlag::allowMultipleCircuits, RtdFlag::allowCableLiftHill, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned, RideMode::poweredLaunch, RideMode::poweredLaunchBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 2, 10 },
    .TrackSpeedSettings = { 60, 60 },
    .BoosterSettings = { 17, 68 },
    .LegacyBoosterSettings = { 17, 68, 2 },
    .Naming = { STR_RIDE_NAME_LSM_LAUNCHED_ROLLER_COASTER, STR_RIDE_DESCRIPTION_LSM_LAUNCHED_ROLLER_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction },
    .Heights = { 33, 24, 9, 11, },
    .MaxMass = 31,
    .LiftData = { Audio::SoundId::liftClassic, 5, 5 },
    .RatingsMultipliers = { 51, 32, 10 },
    .UpkeepCosts = { 10, 20, 80, 12, 3, 40 },
    .BuildCosts = { 57.00_GBP, 2.50_GBP, 55, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectModern,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 120,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::lightBlue, Drawing::Colour::lightBlue, Drawing::Colour::black}, // Taiga
        { Drawing::Colour::brightGreen, Drawing::Colour::brightGreen, Drawing::Colour::yellow}, // Cheetah Hunt
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::beige}, // Maverick
        { Drawing::Colour::black, Drawing::Colour::black, Drawing::Colour::umber}, // Taron
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_GIGA_RC, SPR_RIDE_DESIGN_PREVIEW_GIGA_RC_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "lsm_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(3, 85), RideRating::make(0, 40), RideRating::make(0, 35) },
        14,
        kDynamicRideShelterRating,
        false,
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
