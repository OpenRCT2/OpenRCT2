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
constexpr RideTypeDescriptor ReverserRollerCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionReverserRC,
        .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_SLOPE, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_BRAKES, TRACK_REVERSER},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES | RIDE_TYPE_FLAG_LAYERED_VEHICLE_PREVIEW,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 0, 0, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_REVERSER_ROLLER_COASTER, STR_RIDE_DESCRIPTION_REVERSER_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_REVERSER_ROLLER_COASTER),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 18, 24, 8, 11, },
    .MaxMass = 15,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftWood, 3, 5 },
    .RatingsMultipliers = { 48, 28, 7 },
    .UpkeepCosts = { 39, 20, 80, 10, 3, 10 },
    .BuildCosts = { 27.50_GBP, 3.00_GBP, 37, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_TECHNO,
    .PhotoItem = ShopItem::Photo3,
    .BonusValue = 65,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_LIGHT_BROWN, COLOUR_DARK_BROWN },
        { COLOUR_SATURATED_BROWN, COLOUR_BORDEAUX_RED, COLOUR_SATURATED_BROWN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "reverser_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 40), RIDE_RATING(1, 80), RIDE_RATING(1, 70) },
        19,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,     873, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,        RIDE_RATING(0, 40), RIDE_RATING(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,        187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,        44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,        364088, 655360, 0 },
            { RatingsModifierType::BonusReversals,        6,        RIDE_RATING(0, 20), RIDE_RATING(0, 20), RIDE_RATING(0, 20) },
            { RatingsModifierType::BonusDuration,         150,      26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,        28672, 23831, 49648 },
            { RatingsModifierType::BonusTurns,            0,        26749, 43458, 45749 },
            { RatingsModifierType::BonusDrops,            0,        40777, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,        16705, 30583, 35108 },
            { RatingsModifierType::BonusProximity,        0,        22367, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,        11155, 0, 0 },
            { RatingsModifierType::RequirementReversals,  1,        8, 1, 1 },
            { RatingsModifierType::RequirementLength,     0xC80000, 2, 1, 1 },
            { RatingsModifierType::RequirementNumDrops,   2,        2, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,        28672, 23831, 49648 },
        },
    },
};
// clang-format on
