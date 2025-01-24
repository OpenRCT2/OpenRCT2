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
constexpr RideTypeDescriptor LIMLaunchedRollerCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::limLaunchedRollerCoaster,
        .supportType = MetalSupportType::Tubes,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::flatRollBanking, TrackGroup::verticalLoop, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::twist, TrackGroup::corkscrew, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::curveVertical, TrackGroup::quarterLoop,  TrackGroup::barrelRoll, TrackGroup::slopeCurveBanked, TrackGroup::slopeSteepLong, TrackGroup::halfLoop, TrackGroup::halfLoopMedium, TrackGroup::halfLoopLarge, TrackGroup::corkscrewLarge},
        .extraTrackGroups = {TrackGroup::flatToSteepSlope, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::PoweredLaunchPasstrough, RideMode::PoweredLaunch, RideMode::PoweredLaunchBlockSectioned),
    .DefaultMode = RideMode::PoweredLaunchPasstrough,
    .OperatingSettings = { 10, 31 },
    .BoosterSettings = { 18, 52 },
    .LegacyBoosterSettings = { 18, 52 },
    .Naming = { STR_RIDE_NAME_LIM_LAUNCHED_ROLLER_COASTER, STR_RIDE_DESCRIPTION_LIM_LAUNCHED_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 35, 24, 5, 7, },
    .MaxMass = 18,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 42, 20, 80, 9, 3, 10 },
    .BuildCosts = { 47.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_GREEN, COLOUR_BRIGHT_GREEN, COLOUR_WHITE },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_DARK_BROWN },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_SATURATED_GREEN },
        { COLOUR_BRIGHT_GREEN, COLOUR_SATURATED_GREEN_LIGHT, COLOUR_SATURATED_PURPLE_DARK}, // Joker's Jinx
        { COLOUR_SATURATED_GREEN_LIGHT, COLOUR_GRASS_GREEN_LIGHT , COLOUR_AQUA_DARK}, // Poltergeist
        { COLOUR_GREY, COLOUR_DARK_BROWN , COLOUR_BLACK}, // Flight of Fear
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_LIM_LAUNCHED_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_LIM_LAUNCHED_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "lim_launched_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 90), MakeRideRating(1, 50), MakeRideRating(2, 20) },
        25,
        -1,
        true,
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
