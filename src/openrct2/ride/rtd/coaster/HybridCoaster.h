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
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor HybridCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::hybridCoaster,
        .supportType = WoodenSupportType::Truss,
        .enabledTrackGroups = {TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::liftHillSteep, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::barrelRoll, TrackGroup::poweredLift, TrackGroup::slopeCurveBanked, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::slopeSteepLong, TrackGroup::curveVertical, TrackGroup::quarterLoop, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::slopeCurveLarge, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::inclinedBrakes },
        .extraTrackGroups = {TrackGroup::booster},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports, 
                     RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, 
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .BoosterSettings = { 15, 52 },
    .LegacyBoosterSettings = { 15, 52 },
    .Naming = { STR_RIDE_NAME_HYBRID_COASTER, STR_RIDE_DESCRIPTION_HYBRID_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 43, 24, 13, 13},
    .MaxMass = 18,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftRMC, 5, 11 },
    .RatingsMultipliers = { 52, 36, 10 },
    .UpkeepCosts = { 40, 20, 80, 10, 3, 10 },
    .BuildCosts = { 65.00_GBP, 3.50_GBP, 55},
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock2,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 120,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_ORANGE, COLOUR_LIGHT_ORANGE, COLOUR_DARK_BROWN }, // Medusa
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_SATURATED_BROWN},
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_GREY }, // Storm Chaser
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_WHITE }, // Hakugei
        { COLOUR_BORDEAUX_RED_DARK, COLOUR_BORDEAUX_RED_DARK, COLOUR_DARK_BROWN }, // Zadra
        { COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN, COLOUR_DARK_BROWN }, // Untamed
        { COLOUR_SATURATED_PURPLE_DARK, COLOUR_SATURATED_PURPLE_DARK, COLOUR_SATURATED_BROWN }, // Iron Gwazi
        { COLOUR_BORDEAUX_RED_DARK, COLOUR_BORDEAUX_RED_DARK, COLOUR_BLACK }, // Wildcat's Revenge
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_HYBRID_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_HYBRID_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "hybrid_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(3, 80), MakeRideRating(1, 00), MakeRideRating(0, 45) },
        16,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 05), 0 },
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
// clang-format on
