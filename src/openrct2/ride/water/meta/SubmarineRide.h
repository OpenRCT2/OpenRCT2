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
constexpr RideTypeDescriptor SubmarineRideRTD =
{
    .Category = RIDE_CATEGORY_WATER,
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionSubmarineRide,
        .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_CURVE_VERY_SMALL, TRACK_CURVE_SMALL},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN |
                     RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR |
                     RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | 
                     RIDE_TYPE_FLAG_ALLOW_MORE_VEHICLES_THAN_STATION_FITS,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_SUBMARINE_RIDE, STR_RIDE_DESCRIPTION_SUBMARINE_RIDE },
    .NameConvention = { RideComponentType::Boat, RideComponentType::Track, RideComponentType::DockingPlatform },
    .EnumName = "RIDE_TYPE_SUBMARINE_RIDE",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 16, 0, 3, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 6, 0 },
    .UpkeepCosts = { 50, 1, 0, 4, 0, 0 },
    .BuildCosts = { 35.00_GBP, 2.50_GBP, 5, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = MUSIC_OBJECT_WATER,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 40,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SUBMARINE_RIDE_TRACK, SPR_RIDE_DESIGN_PREVIEW_SUBMARINE_RIDE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "submarine_ride",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 20), RIDE_RATING(1, 80), RIDE_RATING(1, 40) },
        7,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusProximity,        0,                11183, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                22310, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = LightFxAddLightsMagicVehicle_BoatHire,
};
// clang-format on
