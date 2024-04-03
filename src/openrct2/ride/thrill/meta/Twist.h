/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor kTwistRTD =
{
    .AlternateType = kRideTypeNull,
    .Category = RIDE_CATEGORY_THRILL,
    .EnabledTrackPieces = {},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::kFlatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionTwist),
    .Flags = RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS |
                     RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL | RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_FLAT_RIDE |
                     RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT |
                     RIDE_TYPE_FLAG_SINGLE_SESSION | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT | RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY,
    .RideModes = EnumsToFlags(RideMode::Rotation),
    .DefaultMode = RideMode::Rotation,
    .OperatingSettings = { 3, 6, 0, 0, 0, 0, 12, 3 },
    .Naming = { STR_RIDE_NAME_TWIST, STR_RIDE_DESCRIPTION_TWIST },
    .NameConvention = { RideComponentType::Structure, RideComponentType::Structure, RideComponentType::Station },
    .kEnumName = nameof(RIDE_TYPE_TWIST),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 12, 64, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 40, 20, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 45.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 40,
    .ColourPresets = DEFAULT_FLAT_RIDE_COLOUR_PRESET,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "twist",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RIDE_RATING(1, 13), RIDE_RATING(0, 97), RIDE_RATING(1, 90) },
        16,
        0,
        false,
        {
            { RatingsModifierType::BonusRotations,        0,                20, 20, 20 },
            { RatingsModifierType::BonusScenery,          0,                13943, 0, 0 },
        },
    },
};
// clang-format on
