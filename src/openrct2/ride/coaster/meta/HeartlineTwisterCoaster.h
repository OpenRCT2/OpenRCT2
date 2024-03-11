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
constexpr RideTypeDescriptor HeartlineTwisterCoasterRTD =
{
    .AlternateType = RIDE_TYPE_NULL,
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_LIFT_HILL_STEEP, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_HEARTLINE_ROLL, TRACK_HEARTLINE_TRANSFER},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionHeartlineTwisterRC),
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES |
                     RIDE_TYPE_FLAG_ALLOW_REVERSED_TRAINS,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 10, 27, 30, 25, 25, 0 },
    .Naming = { STR_RIDE_NAME_HEARTLINE_TWISTER_COASTER, STR_RIDE_DESCRIPTION_HEARTLINE_TWISTER_COASTER },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_HEARTLINE_TWISTER_COASTER),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 22, 24, 15, 9, },
    .MaxMass = 18,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 47, 20, 80, 11, 3, 10 },
    .BuildCosts = { 72.50_GBP, 3.50_GBP, 30, },
    .DefaultPrices = { 15, 20 },
    .DefaultMusic = MUSIC_OBJECT_TECHNO,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 70,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_LIGHT_BLUE, COLOUR_WHITE },
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_DARK_PURPLE, COLOUR_WHITE, COLOUR_DARK_GREEN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "heartline_twister_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(3, 00), RIDE_RATING(1, 70), RIDE_RATING(1, 65) },
        18,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000, 764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,    RIDE_RATING(0, 20), RIDE_RATING(0, 04), 0 },
            { RatingsModifierType::BonusTrainLength,      0,    187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,    97418, 123987, 70849 },
            { RatingsModifierType::BonusAverageSpeed,     0,    291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,  26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,    24576, 44683, 89367 },
            { RatingsModifierType::BonusTurns,            0,    26749, 52150, 57186 },
            { RatingsModifierType::BonusDrops,            0,    29127, 53052, 55705 },
            { RatingsModifierType::BonusSheltered,        0,    15420, 34952, 35108 },
            { RatingsModifierType::BonusProximity,        0,    9841, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,    3904, 0, 0 },
            { RatingsModifierType::RequirementInversions, 1,    4, 1, 1 },
            { RatingsModifierType::RequirementNumDrops,   1,    4, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,    24576, 44683, 89367 },
        },
    },
};
// clang-format on
