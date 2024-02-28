/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../drawing/LightFX.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor MonsterTrucksRTD =
{
    .AlternateType = RIDE_TYPE_NULL,
    .Category = RIDE_CATEGORY_GENTLE,
    .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_CURVE_VERY_SMALL, TRACK_CURVE_SMALL, TRACK_RAPIDS},
    .ExtraTrackPieces = {TRACK_SPINNING_TUNNEL},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionCarRide),
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS |
                     RIDE_TYPE_FLAG_HAS_DATA_LOGGING | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN |
                     RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR |
                     RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT |
                     RIDE_TYPE_FLAG_ALLOW_MORE_VEHICLES_THAN_STATION_FITS | RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER | 
                     RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT,
    .RideModes = (1uLL << static_cast<uint8_t>(RideMode::ContinuousCircuit)),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 0, 0, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_MONSTER_TRUCKS, STR_RIDE_DESCRIPTION_MONSTER_TRUCKS_GROUP },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_MONSTER_TRUCKS),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 18, 24, 4, 7, },
    .MaxMass = 2,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 10, 10 },
    .UpkeepCosts = { 70, 20, 0, 8, 3, 5 },
    .BuildCosts = { 12.50_GBP, 2.50_GBP, 30, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = MUSIC_OBJECT_SUMMER,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 50,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN, COLOUR_GREY },
        { COLOUR_LIGHT_PURPLE, COLOUR_LIGHT_PURPLE, COLOUR_WHITE },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_OLIVE_GREEN },
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_TRACK, SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "monster_trucks",
        .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 00), RIDE_RATING(0, 50), RIDE_RATING(0, 00) },
        12,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 15), RIDE_RATING(0, 00), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusTurns,            0,                14860, 0, 11437 },
            { RatingsModifierType::BonusDrops,            0,                8738, 0, 0 },
            { RatingsModifierType::BonusSheltered,        0,                12850, 6553, 4681 },
            { RatingsModifierType::BonusProximity,        0,                11183, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                8366, 0, 0 },
            { RatingsModifierType::RequirementLength,     0xC80000,         8, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = LightFxAddLightsMagicVehicle_BoatHire,
};
// clang-format on
