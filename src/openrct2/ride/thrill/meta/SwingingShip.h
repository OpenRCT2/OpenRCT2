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
constexpr RideTypeDescriptor SwingingShipRTD =
{
    .AlternateType = RIDE_TYPE_NULL,
    .Category = RIDE_CATEGORY_THRILL,
    .EnabledTrackPieces = {},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::FlatTrack1x5,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionSwingingShip),
    .Flags = RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS |
                     RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS |
                     RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL | RIDE_TYPE_FLAG_TRACK_NO_WALLS |
                     RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_ALLOW_MUSIC |
                     RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_SINGLE_SESSION | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT |
                     RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY,
    .RideModes = EnumsToFlags(RideMode::Swing),
    .DefaultMode = RideMode::Swing,
    .OperatingSettings = { 7, 25, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_SWINGING_SHIP, STR_RIDE_DESCRIPTION_SWINGING_SHIP },
    .NameConvention = { RideComponentType::Ship, RideComponentType::Structure, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_SWINGING_SHIP),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 12, 112, 7, 11, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 77.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = MUSIC_OBJECT_PIRATE,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 35,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SWINGING_SHIP_TRACK, SPR_RIDE_DESIGN_PREVIEW_SWINGING_SHIP_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "swinging_ship",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RIDE_RATING(1, 50), RIDE_RATING(1, 90), RIDE_RATING(1, 41) },
        10,
        0,
        false,
        {
            { RatingsModifierType::BonusOperationOption, 0, 5, 5, 10 }, 
            { RatingsModifierType::BonusScenery,         0, 16732, 0, 0 },
        },
    },
};
// clang-format on
