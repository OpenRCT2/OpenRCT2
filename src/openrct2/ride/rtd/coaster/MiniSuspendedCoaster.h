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
constexpr RideTypeDescriptor MiniSuspendedCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::miniSuspendedCoaster,
        .supportType = MetalSupportType::Fork,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::slope, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports, 
                     RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, RtdFlag::isSuspended),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_MINI_SUSPENDED_COASTER, STR_RIDE_DESCRIPTION_MINI_SUSPENDED_COASTER },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 10, 24, 24, 8, },
    .MaxMass = 3,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftFrictionWheels, 4, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 10, 3, 10 },
    .BuildCosts = { 30.00_GBP, 2.50_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectWildWest,
    .PhotoItem = ShopItem::Photo2,
    .BonusValue = 50,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_SATURATED_BROWN },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MINI_SUSPENDED_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_MINI_SUSPENDED_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "mini_suspended_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 80), MakeRideRating(2, 50), MakeRideRating(2, 70) },
        15,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 45), MakeRideRating(0, 15), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                24576, 35746, 49648 },
            { RatingsModifierType::BonusTurns,            0,                34179, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                58254, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                19275, 32768, 35108 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                13943, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 6,                2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0x80000,          2, 2, 2 },
            { RatingsModifierType::RequirementLateralGs,  MakeFixed16_2dp(1, 30), 2, 2, 2 },
            { RatingsModifierType::RequirementLength,     0xC80000,         2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 35746, 49648 },
        },
    },
};
// clang-format on
