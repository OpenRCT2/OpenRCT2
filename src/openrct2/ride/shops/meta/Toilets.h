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
constexpr RideTypeDescriptor ToiletsRTD =
{
    .Category = RIDE_CATEGORY_SHOP,
    .StartTrackPiece = TrackElemType::FlatTrack1x1A,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionFacility,
        .EnabledTrackPieces = {},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_NO_TEST_MODE |
                     RIDE_TYPE_FLAG_NO_VEHICLES | RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY | RIDE_TYPE_FLAG_TRACK_NO_WALLS |
                     RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY | RIDE_TYPE_FLAG_IN_RIDE |
                     RIDE_TYPE_FLAG_IS_TOILET | RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY,
    .RideModes = EnumsToFlags(RideMode::ShopStall),
    .DefaultMode = RideMode::ShopStall,
    .OperatingSettings = { 4, 4 },
    .Naming = { STR_RIDE_NAME_TOILETS, STR_RIDE_DESCRIPTION_TOILETS },
    .NameConvention = { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_TOILETS),
    .AvailableBreakdowns = 0,
    .Heights = { 12, DefaultToiletHeight, 0, 0, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 0, 0, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 225.00_GBP, 0.00_GBP, 1, },
    .DefaultPrices = { 0, 0 },
    .DefaultMusic = MUSIC_OBJECT_GENTLE,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 5,
    .ColourPresets = DEFAULT_FLAT_RIDE_COLOUR_PRESET,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Toilets,
    .Name = "toilets",
    .RatingsData = 
    {
        RatingsCalculationType::Stall,
        { 1, 1, 1 },
        1,
        -1,
        false,
        { { RatingsModifierType::NoModifier, 0, 0, 0, 0 } }
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::KioskOrFacility,
};
// clang-format on
