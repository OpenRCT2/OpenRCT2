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
#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor MiniatureRailwayRTD =
{
    .Category = RIDE_CATEGORY_TRANSPORT,
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionMiniatureRailway,
        .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_CURVE_LARGE},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION | RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS |
                     RIDE_TYPE_FLAG_HAS_DATA_LOGGING | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS |
                     RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR |
                     RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_ALLOW_MORE_VEHICLES_THAN_STATION_FITS |
                     RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS | RIDE_TYPE_FLAG_TRANSPORT_RIDE | RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER |
                     RIDE_TYPE_FLAG_SUPPORTS_LEVEL_CROSSINGS,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::Shuttle),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_MINIATURE_RAILWAY, STR_RIDE_DESCRIPTION_MINIATURE_RAILWAY },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_MINIATURE_RAILWAY),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 7, 32, 5, 9, },
    .MaxMass = 39,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 6, -10 },
    .UpkeepCosts = { 60, 20, 0, 10, 3, 5 },
    .BuildCosts = { 17.50_GBP, 3.00_GBP, 50, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = MUSIC_OBJECT_SUMMER,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 50,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_WHITE, COLOUR_DARK_BROWN },
        { COLOUR_BRIGHT_RED, COLOUR_WHITE, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MINIATURE_RAILWAY_TRACK, SPR_RIDE_DESIGN_PREVIEW_MINIATURE_RAILWAY_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "miniature_railway",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 50), RIDE_RATING(0, 00), RIDE_RATING(0, 00) },
        11,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,            6000,     764, 0, 0 },
            { RatingsModifierType::BonusTrainLength,       0,        140434, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,          0,        44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,      0,        291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,          150,      26214, 0, 0 },
            { RatingsModifierType::BonusSheltered,         0,        -6425, 6553, 23405 },
            { RatingsModifierType::BonusProximity,         0,        8946, 0, 0 },
            { RatingsModifierType::BonusScenery,           0,        20915 , 0, 0},
            { RatingsModifierType::RequirementLength,      0xC80000, 2, 2, 2 },
            { RatingsModifierType::RequirementUnsheltered, 4,        4, 1, 1 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = LightFxAddLightsMagicVehicle_MiniatureRailway,
};
// clang-format on
