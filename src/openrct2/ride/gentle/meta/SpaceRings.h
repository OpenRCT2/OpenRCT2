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
constexpr RideTypeDescriptor SpaceRingsRTD =
{
    .AlternateType = RIDE_TYPE_NULL,
    .Category = RIDE_CATEGORY_GENTLE,
    .EnabledTrackPieces = {},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::FlatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionSpaceRings),
    .Flags = RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_NO_TEST_MODE |
                     RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS |
                     RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_SINGLE_SESSION |
                     RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT | RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY,
    .RideModes = EnumsToFlags(RideMode::SpaceRings),
    .DefaultMode = RideMode::SpaceRings,
    .OperatingSettings = { 0, 0, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_SPACE_RINGS, STR_RIDE_DESCRIPTION_SPACE_RINGS },
    .NameConvention = { RideComponentType::Ring, RideComponentType::Structure, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_SPACE_RINGS),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 16, 48, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 12, 4, 4 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 36.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 5, 0 },
    .DefaultMusic = MUSIC_OBJECT_GENTLE,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 30,
    .ColourPresets = DEFAULT_FLAT_RIDE_COLOUR_PRESET,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "space_rings",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RIDE_RATING(1, 50), RIDE_RATING(2, 10), RIDE_RATING(6, 50) },
        7,
        0,
        false,
        {
            { RatingsModifierType::BonusScenery,          0,                25098, 0, 0 },
        },
    },
};
// clang-format on
