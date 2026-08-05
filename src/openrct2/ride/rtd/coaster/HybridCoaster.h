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

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor HybridCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::hybridCoaster,
        .supportType = WoodenSupportType::truss,
        .enabledTrackGroups = { TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::liftHillSteep, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::barrelRoll, TrackGroup::poweredLift, TrackGroup::slopeCurveBanked, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::slopeSteepLong, TrackGroup::curveVertical, TrackGroup::quarterLoop, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::slopeCurveLarge, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::inclinedBrakes, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown, TrackGroup::helixUpUnbankedQuarter, TrackGroup::helixDownUnbankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::helixDownBankedQuarter },
        .extraTrackGroups = {TrackGroup::booster},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports,
                     RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 15, 52 },
    .LegacyBoosterSettings = { 15, 52 },
    .Naming = { STR_RIDE_NAME_HYBRID_COASTER, STR_RIDE_DESCRIPTION_HYBRID_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 43, 24, 13, 13},
    .MaxMass = 18,
    .LiftData = { Audio::SoundId::liftRMC, 5, 11 },
    .RatingsMultipliers = { 52, 36, 10 },
    .UpkeepCosts = { 40, 20, 80, 10, 3, 10 },
    .BuildCosts = { 65.00_GBP, 3.50_GBP, 55},
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock2,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 120,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::lightOrange, Drawing::Colour::lightOrange, Drawing::Colour::darkBrown }, // Medusa
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::saturatedBrown},
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::grey }, // Storm Chaser
        { Drawing::Colour::lightBlue, Drawing::Colour::lightBlue, Drawing::Colour::white }, // Hakugei
        { Drawing::Colour::maroon, Drawing::Colour::maroon, Drawing::Colour::darkBrown }, // Zadra
        { Drawing::Colour::saturatedBrown, Drawing::Colour::saturatedBrown, Drawing::Colour::darkBrown }, // Untamed
        { Drawing::Colour::violet, Drawing::Colour::violet, Drawing::Colour::saturatedBrown }, // Iron Gwazi
        { Drawing::Colour::maroon, Drawing::Colour::maroon, Drawing::Colour::black }, // Wildcat's Revenge
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_HYBRID_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_HYBRID_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "hybrid_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(3, 80), RideRating::make(1, 00), RideRating::make(0, 45) },
        16,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                364088, 400497, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                40960, 35746, 49648 },
            { RatingsModifierType::bonusTurns,            0,                34179, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                34952, 46811, 49152 },
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
