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
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor LSMLaunchedRollerCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::latticeTriangleAlt,
        .supportType = MetalSupportType::Tubes,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::onridePhoto, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::slopeSteepLong, TrackGroup::slopeCurveBanked, TrackGroup::slopeCurveLarge, TrackGroup::slopeCurveLargeBanked, TrackGroup::booster, TrackGroup::diagBooster, TrackGroup::poweredLift, TrackGroup::slopeVertical, TrackGroup::curveVertical, TrackGroup::barrelRoll, TrackGroup::quarterLoop, TrackGroup::halfLoop, TrackGroup::halfLoopMedium, TrackGroup::halfLoopLarge, TrackGroup::verticalLoop, TrackGroup::corkscrew, TrackGroup::corkscrewLarge, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::diagSlopeSteepLong, TrackGroup::diveLoop},
        .extraTrackGroups = {TrackGroup::liftHill, TrackGroup::liftHillCable},
        }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
                 EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, 
                              RtdFlag::allowMultipleCircuits, RtdFlag::allowCableLiftHill, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned, RideMode::PoweredLaunch, RideMode::PoweredLaunchBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 2, 10 },
    .TrackSpeedSettings = { 60, 60 },
    .BoosterSettings = { 17, 68 },
    .LegacyBoosterSettings = { 17, 68, 2 },
    .Naming = { STR_RIDE_NAME_LSM_LAUNCHED_ROLLER_COASTER, STR_RIDE_DESCRIPTION_LSM_LAUNCHED_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 33, 24, 9, 11, },
    .MaxMass = 31,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 5, 5 },
    .RatingsMultipliers = { 51, 32, 10 },
    .UpkeepCosts = { 10, 20, 80, 12, 3, 40 },
    .BuildCosts = { 57.00_GBP, 2.50_GBP, 55, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectModern,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 120,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_BLACK}, // Taiga
        { COLOUR_BRIGHT_GREEN, COLOUR_BRIGHT_GREEN, COLOUR_YELLOW}, // Cheetah Hunt
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_DULL_BROWN_LIGHT}, // Maverick
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_DULL_BROWN_DARK}, // Taron
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_GIGA_RC, SPR_RIDE_DESIGN_PREVIEW_GIGA_RC_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "lsm_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(3, 85), MakeRideRating(0, 40), MakeRideRating(0, 35) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                24576, 35746, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 15, 20 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 10,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, 10,               2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 35746, 49648 },
        },
    },
};
// clang-format on
