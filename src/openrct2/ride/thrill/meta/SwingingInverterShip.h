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
constexpr RideTypeDescriptor SwingingInverterShipRTD =
{
    .Category = RIDE_CATEGORY_THRILL,
    .StartTrackPiece = TrackElemType::FlatTrack1x4B,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionSwingingInverterShip,
        .EnabledTrackPieces = {},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION |
                     RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL |
                     RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS |
                     RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_SINGLE_SESSION |
                     RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT | RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY,
    .RideModes = EnumsToFlags(RideMode::Swing),
    .DefaultMode = RideMode::Swing,
    .OperatingSettings = { 7, 15 },
    .Naming = { STR_RIDE_NAME_SWINGING_INVERTER_SHIP, STR_RIDE_DESCRIPTION_SWINGING_INVERTER_SHIP },
    .NameConvention = { RideComponentType::Ship, RideComponentType::Structure, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_SWINGING_INVERTER_SHIP),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 15, 176, 7, 11, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 106.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = MUSIC_OBJECT_TECHNO,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 35,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_BLACK, COLOUR_SATURATED_BROWN, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SWINGING_INVERTER_SHIP_TRACK, SPR_RIDE_DESIGN_PREVIEW_SWINGING_INVERTER_SHIP_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "swinging_inverter_ship",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RIDE_RATING(2, 50), RIDE_RATING(2, 70), RIDE_RATING(2, 74) },
        16,
        0,
        false,
        {
            { RatingsModifierType::BonusOperationOption, 0, 11, 22, 22 }, 
            { RatingsModifierType::BonusScenery,         0, 11155, 0, 0 },
        },
    },
};
// clang-format on
