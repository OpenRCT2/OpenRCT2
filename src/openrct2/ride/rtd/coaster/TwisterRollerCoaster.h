/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../paint/support/MetalSupports.h"
#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor TwisterRollerCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::twisterRollerCoaster,
        .supportType = MetalSupportType::Tubes,
        .enabledTrackGroups = {TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::verticalLoop, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::halfLoop, TrackGroup::corkscrew, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::barrelRoll, TrackGroup::poweredLift, TrackGroup::halfLoopLarge, TrackGroup::slopeCurveBanked, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::slopeSteepLong, TrackGroup::curveVertical, TrackGroup::quarterLoop, TrackGroup::booster, TrackGroup::twist, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::slopeCurveLarge, TrackGroup::corkscrewLarge, TrackGroup::halfLoopMedium, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::diagSlopeSteepLong, TrackGroup::diveLoop},
        .extraTrackGroups = {TrackGroup::liftHillSteep, TrackGroup::brakeForDrop},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, 
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 17, 68 },
    .LegacyBoosterSettings = { 17, 68 },
    .Naming = { STR_RIDE_NAME_TWISTER_ROLLER_COASTER, STR_RIDE_DESCRIPTION_TWISTER_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 40, 24, 8, 9, },
    .MaxMass = 31,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftBM, 5, 8 },
    .RatingsMultipliers = { 52, 36, 10 },
    .UpkeepCosts = { 43, 20, 80, 11, 3, 10 },
    .BuildCosts = { 60.00_GBP, 2.50_GBP, 55, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 120,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BORDEAUX_RED },
        { COLOUR_AQUAMARINE, COLOUR_AQUAMARINE, COLOUR_DARK_PURPLE },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_LIGHT_BLUE },
        { COLOUR_DARK_GREEN, COLOUR_MOSS_GREEN, COLOUR_DARK_BROWN },
        { COLOUR_BORDEAUX_RED, COLOUR_LIGHT_ORANGE, COLOUR_WHITE },
        { COLOUR_LIGHT_BLUE, COLOUR_YELLOW, COLOUR_DARK_PINK }, // Scream
        { COLOUR_DARK_ORANGE, COLOUR_DARK_ORANGE, COLOUR_BLACK }, // Rougarou
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_TWISTER_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_TWISTER_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "twister_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(3, 50), MakeRideRating(0, 40), MakeRideRating(0, 30) },
        15,
        -1,
        true,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                24576, 32768, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 12, 20 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 12,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 32768, 49648 },
        },
    },
};
// clang-format on
