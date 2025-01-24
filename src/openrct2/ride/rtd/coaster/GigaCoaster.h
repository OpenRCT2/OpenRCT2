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
constexpr RideTypeDescriptor GigaCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::latticeTriangle,
        .supportType = MetalSupportType::Tubes,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::slopeSteepLong, TrackGroup::liftHillCable, TrackGroup::slopeCurveBanked, TrackGroup::slopeCurveLarge, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::slopeVertical, TrackGroup::curveVertical, TrackGroup::diagSlopeSteepLong},
        .extraTrackGroups = {TrackGroup::barrelRoll, TrackGroup::quarterLoop, TrackGroup::halfLoop, TrackGroup::halfLoopMedium, TrackGroup::halfLoopLarge, TrackGroup::verticalLoop, TrackGroup::corkscrew, TrackGroup::corkscrewLarge, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::booster, TrackGroup::diagBooster, TrackGroup::diveLoop, TrackGroup::poweredLift},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
                 EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, 
                              RtdFlag::allowMultipleCircuits, RtdFlag::allowCableLiftHill, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 10, 27 },
    .TrackSpeedSettings = { 60, 60 },
    .BoosterSettings = { 17, 68 },
    .LegacyBoosterSettings = { 17, 68, 4 },
    .Naming = { STR_RIDE_NAME_GIGA_COASTER, STR_RIDE_DESCRIPTION_GIGA_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 86, 24, 9, 11, },
    .MaxMass = 31,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 5, 8 },
    .RatingsMultipliers = { 51, 32, 10 },
    .UpkeepCosts = { 10, 20, 80, 12, 3, 40 },
    .BuildCosts = { 60.00_GBP, 2.50_GBP, 55, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock3,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 120,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_WHITE },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_YELLOW },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BORDEAUX_RED },
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_BLACK },
        { COLOUR_BRIGHT_YELLOW, COLOUR_YELLOW, COLOUR_DARK_GREEN }, // Mega-Lite / 蓝月飞车
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_GIGA_RC, SPR_RIDE_DESIGN_PREVIEW_GIGA_RC_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "giga_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(3, 85), MakeRideRating(0, 40), MakeRideRating(0, 35) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             819, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                140434, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                51366, 85019, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                364088, 400497, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                36864, 30384, 49648 },
            { RatingsModifierType::BonusTurns,            0,                28235, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                43690, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 20, 20 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 16,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                36864, 30384, 49648 },
        },
    },
};
// clang-format on
