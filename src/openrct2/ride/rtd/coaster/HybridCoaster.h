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
#include "../../Track.h"

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
        .enabledTrackGroups = { TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::liftHillSteep, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::barrelRoll, TrackGroup::poweredLift, TrackGroup::slopeCurveBanked, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::slopeSteepLong, TrackGroup::curveVertical, TrackGroup::quarterLoop, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::slopeCurveLarge, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::inclinedBrakes, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown },
        .extraTrackGroups = {TrackGroup::booster},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports, 
                     RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, 
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .BoosterSettings = { 15, 52 },
    .LegacyBoosterSettings = { 15, 52 },
    .Naming = { STR_RIDE_NAME_HYBRID_COASTER, STR_RIDE_DESCRIPTION_HYBRID_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
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
    .ColourKey = RideColourKey::Ride,
    .Name = "hybrid_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(3, 80), RideRating::make(1, 00), RideRating::make(0, 45) },
        16,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                364088, 400497, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                40960, 35746, 49648 },
            { RatingsModifierType::BonusTurns,            0,                34179, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                34952, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 15, 25 },
            { RatingsModifierType::BonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 14,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 35746, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
