/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionLimLaunchedRC,
        .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_FLAT_ROLL_BANKING, TRACK_VERTICAL_LOOP, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE, TRACK_SLOPE_CURVE_STEEP, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_CURVE_LARGE, TRACK_TWIST, TRACK_CORKSCREW, TRACK_HELIX_DOWN_BANKED_HALF, TRACK_HELIX_UP_BANKED_HALF, TRACK_BRAKES, TRACK_ON_RIDE_PHOTO, TRACK_SLOPE_VERTICAL, TRACK_BLOCK_BRAKES, TRACK_SLOPE_ROLL_BANKING, TRACK_CURVE_VERTICAL, TRACK_QUARTER_LOOP,  TRACK_BARREL_ROLL, TRACK_SLOPE_CURVE_BANKED, TRACK_SLOPE_STEEP_LONG, TRACK_HALF_LOOP, TRACK_HALF_LOOP_MEDIUM, TRACK_HALF_LOOP_LARGE, TRACK_CORKSCREW_LARGE},
        .ExtraTrackPieces = {TRACK_SLOPE_LONG, TRACK_ZERO_G_ROLL, TRACK_ZERO_G_ROLL_LARGE},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT |
                     RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES | RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS |
                     RIDE_TYPE_FLAG_ALLOW_REVERSED_TRAINS,
    .RideModes = EnumsToFlags(RideMode::PoweredLaunchPasstrough, RideMode::PoweredLaunch, RideMode::PoweredLaunchBlockSectioned),
    .DefaultMode = RideMode::PoweredLaunchPasstrough,
    .OperatingSettings = { 10, 31 },
    .BoosterSettings = { 18, 52 },
    .LegacyBoosterSettings = { 18, 52 },
    .Naming = { STR_RIDE_NAME_LIM_LAUNCHED_ROLLER_COASTER, STR_RIDE_DESCRIPTION_LIM_LAUNCHED_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 35, 24, 5, 7, },
    .MaxMass = 18,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 42, 20, 80, 9, 3, 10 },
    .BuildCosts = { 47.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_ROCK_1,
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
        { RIDE_RATING(2, 90), RIDE_RATING(1, 50), RIDE_RATING(2, 20) },
        25,
        -1,
        true,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 40), RIDE_RATING(0, 05), 0 },
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
