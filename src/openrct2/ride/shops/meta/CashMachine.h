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
constexpr RideTypeDescriptor CashMachineRTD =
{
    .Category = RIDE_CATEGORY_SHOP,
    .StartTrackPiece = TrackElemType::FlatTrack1x1A,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionShop,
        .EnabledTrackPieces = {},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_NO_TEST_MODE |
                     RIDE_TYPE_FLAG_NO_VEHICLES | RIDE_TYPE_FLAG_IS_CASH_MACHINE | RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY |
                     RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY,
    .RideModes = EnumsToFlags(RideMode::ShopStall),
    .DefaultMode = RideMode::ShopStall,
    .OperatingSettings = { 0, 0, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_CASH_MACHINE, STR_RIDE_DESCRIPTION_CASH_MACHINE },
    .NameConvention = { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_CASH_MACHINE),
    .AvailableBreakdowns = 0,
    .Heights = { 12, DefaultCashMachineHeight, 0, 0, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 0, 0, 0 },
    .UpkeepCosts = { 40, 1, 0, 0, 0, 0 },
    .BuildCosts = { 200.00_GBP, 0.00_GBP, 1, },
    .DefaultPrices = { 0, 0 },
    .DefaultMusic = MUSIC_OBJECT_GENTLE,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 5,
    .ColourPresets = DEFAULT_FLAT_RIDE_COLOUR_PRESET,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::CashMachine,
    .Name = "cash_machine",
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
