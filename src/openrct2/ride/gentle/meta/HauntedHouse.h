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
constexpr RideTypeDescriptor HauntedHouseRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = TrackElemType::FlatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionHauntedHouse,
        .EnabledTrackPieces = {},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS |
                     RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL | RIDE_TYPE_FLAG_TRACK_NO_WALLS |
                     RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_IN_RIDE | RIDE_TYPE_FLAG_ALLOW_MUSIC |
                     RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_SINGLE_SESSION |
                     RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY,
    .RideModes = EnumsToFlags(RideMode::HauntedHouse),
    .DefaultMode = RideMode::HauntedHouse,
    .Naming = { STR_RIDE_NAME_HAUNTED_HOUSE, STR_RIDE_DESCRIPTION_HAUNTED_HOUSE },
    .NameConvention = { RideComponentType::Building, RideComponentType::Structure, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_HAUNTED_HOUSE),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 16, 160, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 20, 10, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 42.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = MUSIC_OBJECT_HORROR,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 22,
    .ColourPresets = DEFAULT_FLAT_RIDE_COLOUR_PRESET,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "haunted_house",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RIDE_RATING(3, 41), RIDE_RATING(1, 53), RIDE_RATING(0, 10) },
        8,
        7,
        false,
        { { RatingsModifierType::NoModifier, 0, 0, 0, 0 } }
    },
};
// clang-format on
