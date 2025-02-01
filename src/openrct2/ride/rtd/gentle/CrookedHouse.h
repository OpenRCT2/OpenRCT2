/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
constexpr RideTypeDescriptor CrookedHouseRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::crookedHouse,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps,
                     RtdFlag::hasLoadOptions, RtdFlag::vehicleIsIntegral, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::describeAsInside, RtdFlag::allowMusic,
                     RtdFlag::hasEntranceAndExit, RtdFlag::singleSession, 
                     RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::CrookedHouse),
    .DefaultMode = RideMode::CrookedHouse,
    .Naming = { STR_RIDE_NAME_CROOKED_HOUSE, STR_RIDE_DESCRIPTION_CROOKED_HOUSE },
    .NameConvention = { RideComponentType::Building, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = 0,
    .Heights = { 16, 96, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 15, 8, 0 },
    .UpkeepCosts = { 30, 1, 0, 0, 0, 0 },
    .BuildCosts = { 32.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 6, 0 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 22,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BLACK, COLOUR_GREY },
    ),
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "crooked_house",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { MakeRideRating(2, 15), MakeRideRating(0, 62), MakeRideRating(0, 34) },
        5,
        7,
        false,
        { { RatingsModifierType::NoModifier, 0, 0, 0, 0 } }
    },
};
// clang-format on
