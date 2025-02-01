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
constexpr RideTypeDescriptor ClassicStandUpRollerCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::classicStandUpRollerCoaster,
        .supportType = MetalSupportType::Tubes,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::verticalLoop, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::halfLoop, TrackGroup::corkscrew, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::blockBrakes, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::slopeSteepLong, TrackGroup::slopeCurveLarge, TrackGroup::corkscrewLarge, TrackGroup::halfLoopMedium, TrackGroup::halfLoopLarge},
        .extraTrackGroups = {TrackGroup::slopeVertical, TrackGroup::slopeCurveSteep, TrackGroup::curveVertical, TrackGroup::quarterLoop, TrackGroup::barrelRoll, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, 
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned, RideMode::ReverseInclineLaunchedShuttle, RideMode::PoweredLaunchPasstrough, RideMode::PoweredLaunch),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 7, 27 },
    .Naming = { STR_RIDE_NAME_CLASSIC_STAND_UP_ROLLER_COASTER, STR_RIDE_DESCRIPTION_CLASSIC_STAND_UP_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 30, 24, 9, 11, },
    .MaxMass = 18,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 10, 3, 10 },
    .BuildCosts = { 50.00_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 90,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BLACK },
        { COLOUR_BLACK, COLOUR_LIGHT_BLUE, COLOUR_BLACK },
        { COLOUR_LIGHT_ORANGE, COLOUR_WHITE, COLOUR_LIGHT_ORANGE },
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BORDEAUX_RED },
        { COLOUR_MOSS_GREEN, COLOUR_MOSS_GREEN, COLOUR_WHITE },
        { COLOUR_SATURATED_BROWN, COLOUR_LIGHT_ORANGE, COLOUR_SATURATED_BROWN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_STAND_UP_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_STAND_UP_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "classic_stand_up_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 50), MakeRideRating(3, 00), MakeRideRating(3, 00) },
        17,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 10), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 123987, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                24576, 35746, 59578 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                34952, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                12850, 28398, 30427 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 20, 30 },
            { RatingsModifierType::BonusProximity,        0,                17893, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                5577, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 12,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 50), 2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 35746, 59578 },
        },
    },
};
// clang-format on
