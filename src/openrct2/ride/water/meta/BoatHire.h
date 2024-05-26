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
constexpr RideTypeDescriptor BoatHireRTD =
{
    .Category = RIDE_CATEGORY_WATER,
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionBoatHire,
        .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_CURVE_VERY_SMALL},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS | RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER |
                     RIDE_TYPE_FLAG_NO_TEST_MODE | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_CHECK_FOR_STALLING |
                     RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT,
    .RideModes = EnumsToFlags(RideMode::BoatHire),
    .DefaultMode = RideMode::BoatHire,
    .Naming = { STR_RIDE_NAME_BOAT_HIRE, STR_RIDE_DESCRIPTION_BOAT_HIRE },
    .NameConvention = { RideComponentType::Boat, RideComponentType::DockingPlatform, RideComponentType::DockingPlatform },
    .EnumName = nameof(RIDE_TYPE_BOAT_HIRE),
    .AvailableBreakdowns = (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 16, 0, 3, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 6, 0 },
    .UpkeepCosts = { 50, 1, 0, 4, 0, 0 },
    .BuildCosts = { 27.50_GBP, 0.00_GBP, 5, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = MUSIC_OBJECT_WATER,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 40,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_MOSS_GREEN, COLOUR_MOSS_GREEN, COLOUR_WHITE },
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_DARK_GREEN },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_YELLOW },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED },
        { COLOUR_ICY_BLUE, COLOUR_ICY_BLUE, COLOUR_ICY_BLUE },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_DARK_BLUE },
        { COLOUR_BRIGHT_PURPLE, COLOUR_BRIGHT_PURPLE, COLOUR_WHITE },
        { COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_YELLOW, COLOUR_YELLOW },
        { COLOUR_DARK_WATER, COLOUR_DARK_WATER, COLOUR_DARK_GREEN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_BOAT_HIRE_TRACK, SPR_RIDE_DESIGN_PREVIEW_BOAT_HIRE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "boat_hire",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(1, 90), RIDE_RATING(0, 80), RIDE_RATING(0, 90) },
        7,
        0,
        false,
        {
            { RatingsModifierType::BonusBoatHireNoCircuit, 0, RIDE_RATING(0, 20), 0, 0 },
            { RatingsModifierType::BonusProximity,         0, 11183, 0, 0 },
            { RatingsModifierType::BonusScenery,           0, 22310, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = LightFxAddLightsMagicVehicle_BoatHire,
};
// clang-format on
