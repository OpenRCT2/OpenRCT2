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
constexpr RideTypeDescriptor kFoodStallRTD =
{
    .AlternateType = kRideTypeNull,
    .Category = RIDE_CATEGORY_SHOP,
    .EnabledTrackPieces = {},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::kFlatTrack1x1A,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionShop),
    .Flags = RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_NO_TEST_MODE |
                     RIDE_TYPE_FLAG_NO_VEHICLES | RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY | RIDE_TYPE_FLAG_TRACK_NO_WALLS |
                     RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_SELLS_FOOD | RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY |
                     RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN,
    .RideModes = EnumsToFlags(RideMode::ShopStall),
    .DefaultMode = RideMode::ShopStall,
    .OperatingSettings = { 0, 0, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_FOOD_STALL, STR_RIDE_DESCRIPTION_FOOD_STALL },
    .NameConvention = { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station },
    .kEnumName = nameof(RIDE_TYPE_FOOD_STALL),
    .AvailableBreakdowns = 0,
    .Heights = { 12, kDefaultFoodStallHeight, 0, 0, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 0, 0, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 300.00_GBP, 0.00_GBP, 1, },
    .DefaultPrices = { 0, 0 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 15,
    .ColourPresets = DEFAULT_STALL_COLOUR_PRESET,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Food,
    .Name = "food_stall",
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
    .Classification = RideClassification::ShopOrStall,
};
// clang-format on
