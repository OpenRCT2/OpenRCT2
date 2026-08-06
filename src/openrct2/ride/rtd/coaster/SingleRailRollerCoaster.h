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

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor SingleRailRollerCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::singleRailRollerCoaster,
        .supportType = MetalSupportType::tubes,
        .enabledTrackGroups = { TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::liftHillSteep, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::flatToSteepSlope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::corkscrew, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::barrelRoll, TrackGroup::slopeCurveBanked, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::curveVertical, TrackGroup::quarterLoop, TrackGroup::halfLoop, TrackGroup::halfLoopLarge, TrackGroup::halfLoopMedium, TrackGroup::corkscrewLarge, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::slopeCurveLarge, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::inclinedBrakes, TrackGroup::diveLoop, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown, TrackGroup::helixUpUnbankedQuarter, TrackGroup::helixDownUnbankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::helixDownBankedQuarter },
        .extraTrackGroups =  {TrackGroup::slopeSteepLong, TrackGroup::diagSlopeSteepLong},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt|
                 RtdFlags(RtdFlag::hasTrackColourSupports, RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                              RtdFlag::checkGForces, RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 7, 27 },
    .BoosterSettings = { 15, 52 },
    .LegacyBoosterSettings = { 15, 52 },
    .Naming = { STR_RIDE_NAME_SINGLE_RAIL_ROLLER_COASTER, STR_RIDE_DESCRIPTION_SINGLE_RAIL_ROLLER_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 28, 24, 5, 7},
    .MaxMass = 18,
    .LiftData = { Audio::SoundId::liftRMC, 5, 8 },
    .RatingsMultipliers = { 52, 36, 10 },
    .UpkeepCosts = { 40, 20, 80, 10, 3, 10 },
    .BuildCosts = { 37.50_GBP, 2.50_GBP, 50},
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 80,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::lightOrange, Drawing::Colour::lightOrange, Drawing::Colour::white },
        { Drawing::Colour::yellow, Drawing::Colour::yellow, Drawing::Colour::black },
        { Drawing::Colour::lightBlue, Drawing::Colour::lightBlue, Drawing::Colour::darkGreen }
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SINGLE_RAIL_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_SINGLE_RAIL_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "single_rail_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(3, 50), RideRating::make(0, 60), RideRating::make(0, 40) },
        16,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                364088, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                36864, 35746, 49648 },
            { RatingsModifierType::bonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::bonusReversedTrains,   0,                2, 15, 25 },
            { RatingsModifierType::bonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 14,               2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::requirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 2, 2 },
            { RatingsModifierType::requirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                24576, 35746, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
