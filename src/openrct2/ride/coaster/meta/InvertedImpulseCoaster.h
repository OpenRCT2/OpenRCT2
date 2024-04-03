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
constexpr RideTypeDescriptor kInvertedImpulseCoasterRTD =
{
    .AlternateType = kRideTypeNull,
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_VERTICAL, TRACK_CURVE_VERTICAL},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::kEndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionInvertedImpulseRC),
    .Flags = kRideTypeFagsTrackHas3Colours | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     kRideTypeFlagsCommonCoaster | kRideTypeFlagsCommonCoasterNonAlt | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES |
                     RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS | RIDE_TYPE_FLAG_IS_SUSPENDED |
                     RIDE_TYPE_FLAG_ALLOW_REVERSED_TRAINS,
    .RideModes = EnumsToFlags(RideMode::PoweredLaunchPasstrough, RideMode::PoweredLaunch),
    .DefaultMode = RideMode::PoweredLaunchPasstrough,
    .OperatingSettings = { 10, 33, 30, 25, 25, 0 },
    .Naming = { STR_RIDE_NAME_INVERTED_IMPULSE_COASTER, STR_RIDE_DESCRIPTION_INVERTED_IMPULSE_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .kEnumName = nameof(RIDE_TYPE_INVERTED_IMPULSE_COASTER),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 45, 40, 29, 8, },
    .MaxMass = 23,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 4, 7 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 180, 20, 80, 11, 3, 10 },
    .BuildCosts = { 62.50_GBP, 2.50_GBP, 25, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo2,
    .BonusValue = 75,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_GREY },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_WHITE },
        { COLOUR_BRIGHT_PURPLE, COLOUR_BRIGHT_PURPLE, COLOUR_BORDEAUX_RED },
        { COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_YELLOW, COLOUR_BORDEAUX_RED }, // Volcano: The Blast Coaster
        { COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_YELLOW, COLOUR_TEAL }, // Wicked Twister
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_INVERTED_IMPULSE_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_INVERTED_IMPULSE_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "inverted_impulse_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(4, 00), RIDE_RATING(3, 00), RIDE_RATING(3, 20) },
        20,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 42), RIDE_RATING(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                24576, 29789, 55606 },
            { RatingsModifierType::BonusTurns,            0,                26749, 29552, 57186 },
            { RatingsModifierType::BonusDrops,            0,                29127, 39009, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 15291, 35108 },
            { RatingsModifierType::BonusProximity,        0,                15657, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                9760, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 20,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 29789, 55606 },
        },
    },
};
// clang-format on
