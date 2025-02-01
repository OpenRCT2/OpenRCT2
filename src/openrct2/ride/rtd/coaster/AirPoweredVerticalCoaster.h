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
constexpr RideTypeDescriptor AirPoweredVerticalCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::airPoweredVerticalCoaster,
        .supportType = WoodenSupportType::Truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHillSteep, TrackGroup::liftHillCurve, TrackGroup::flatRollBanking, TrackGroup::curve, TrackGroup::brakes, TrackGroup::reverseFreefall, TrackGroup::slopeToFlat, TrackGroup::onridePhoto},
        .extraTrackGroups = {TrackGroup::booster},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | EnumsToFlags(RtdFlag::checkGForces),
    .RideModes = EnumsToFlags(RideMode::PoweredLaunchPasstrough, RideMode::PoweredLaunch),
    .DefaultMode = RideMode::PoweredLaunchPasstrough,
    .OperatingSettings = { 30, 50 },
    .TrackSpeedSettings = { 60, 60 },
    .BoosterSettings = { 40, 40, 13 },
    .LegacyBoosterSettings = { 40, 40 },
    .Naming = { STR_RIDE_NAME_AIR_POWERED_VERTICAL_COASTER, STR_RIDE_DESCRIPTION_AIR_POWERED_VERTICAL_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 32, 4, 7, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 44, 66, 10 },
    .UpkeepCosts = { 90, 20, 0, 0, 0, 10 },
    .BuildCosts = { 100.00_GBP, 0.00_GBP, 45, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock2,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 70,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_BRIGHT_RED, COLOUR_DARK_BROWN },
        { COLOUR_BRIGHT_RED, COLOUR_LIGHT_BLUE, COLOUR_WHITE },
        { COLOUR_LIGHT_ORANGE, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_YELLOW },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_AIR_POWERED_VERTICAL_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_AIR_POWERED_VERTICAL_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "air_powered_vertical_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(4, 13), MakeRideRating(2, 50), MakeRideRating(2, 80) },
        28,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             327, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 60), MakeRideRating(0, 05), 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                509724, 364088, 320398 },
            { RatingsModifierType::BonusGForces,          0,                24576, 35746, 59578 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 21845, 11702 },
            { RatingsModifierType::BonusProximity,        0,                17893, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 34,               4, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 35746, 59578 },
        },
    },
};
// clang-format on
