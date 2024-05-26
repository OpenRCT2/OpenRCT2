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
constexpr RideTypeDescriptor TopSpinRTD =
{
    .Category = RIDE_CATEGORY_THRILL,
    .StartTrackPiece = TrackElemType::FlatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionTopspin,
        .EnabledTrackPieces = {},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION |
                     RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL |
                     RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS |
                     RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_SINGLE_SESSION |
                     RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT | RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY,
    .RideModes = EnumsToFlags(RideMode::Beginners, RideMode::Intense, RideMode::Berserk),
    .DefaultMode = RideMode::Beginners,
    .Naming = { STR_RIDE_NAME_TOP_SPIN, STR_RIDE_DESCRIPTION_TOP_SPIN },
    .NameConvention = { RideComponentType::Car, RideComponentType::Structure, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_TOP_SPIN),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 16, 112, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 24, 20, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 72.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 20, 0 },
    .DefaultMusic = MUSIC_OBJECT_TECHNO,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_YELLOW, COLOUR_YELLOW },
        { COLOUR_TEAL, COLOUR_WHITE, COLOUR_WHITE },
        { COLOUR_DARK_GREEN, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_TOP_SPIN_TRACK, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "top_spin",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { 1, 1, 1 },
        19,
        0,
        false,
        {
            { RatingsModifierType::BonusTopSpinMode, 0, 0, 0, 0 },
            { RatingsModifierType::BonusScenery,     0, 11155, 0, 0 },
        },
    },
};
// clang-format on
