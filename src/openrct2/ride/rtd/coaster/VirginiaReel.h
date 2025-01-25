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
constexpr RideTypeDescriptor VirginiaReelRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::virginiaReel,
        .supportType = WoodenSupportType::Truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::slope, TrackGroup::curveVerySmall, TrackGroup::curveSmall},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
         EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_VIRGINIA_REEL, STR_RIDE_DESCRIPTION_VIRGINIA_REEL },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 14, 24, 6, 7, },
    .MaxMass = 15,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 3, 5 },
    .RatingsMultipliers = { 30, 15, 25 },
    .UpkeepCosts = { 39, 20, 80, 10, 3, 10 },
    .BuildCosts = { 26.50_GBP, 3.00_GBP, 25, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo3,
    .BonusValue = 50,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_SATURATED_BROWN, COLOUR_GREY, COLOUR_SATURATED_BROWN },
        { COLOUR_BLACK, COLOUR_DARK_BROWN, COLOUR_DARK_BROWN },
        { COLOUR_DARK_BROWN, COLOUR_BORDEAUX_RED, COLOUR_DARK_BROWN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_VIRGINIA_REEL_TRACK, SPR_RIDE_DESIGN_PREVIEW_VIRGINIA_REEL_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "virginia_reel",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 10), MakeRideRating(1, 90), MakeRideRating(3, 70) },
        19,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             873, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                364088, 655360, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                110592, 29789, 59578 },
            { RatingsModifierType::BonusTurns,            0,                52012, 26075, 45749 },
            { RatingsModifierType::BonusDrops,            0,                43690, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::BonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::RequirementLength,     0xD20000,         2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                110592, 29789, 59578 },
        },
    },
};
// clang-format on
