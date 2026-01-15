/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
namespace OpenRCT2
{
constexpr RideTypeDescriptor DrinkStallRTD =
{
    .Category = RideCategory::shop,
    .StartTrackPiece = TrackElemType::flatTrack1x1A,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::shop,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps, RtdFlag::noTestMode,
                     RtdFlag::noVehicles, RtdFlag::isShopOrFacility, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::sellsDrinks, RtdFlag::listVehiclesSeparately,
                     RtdFlag::hasTrackColourMain),
    .RideModes = EnumsToFlags(RideMode::shopStall),
    .DefaultMode = RideMode::shopStall,
    .Naming = { STR_RIDE_NAME_DRINK_STALL, STR_RIDE_DESCRIPTION_DRINK_STALL },
    .NameConvention = { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station },
    .AvailableBreakdowns = 0,
    .Heights = { 12, kDefaultDrinksStallHeight, 0, 0, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 0, 0, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 250.00_GBP, 0.00_GBP, 1, },
    .DefaultPrices = { 0, 0 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 15,
    .ColourPresets = kDefaultStallColourPreset,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Drink,
    .Name = "drink_stall",
    .RatingsData = 
    {
        RatingsCalculationType::Stall,
        { 1, 1, 1 },
        1,
        kDynamicRideShelterRating,
        false,
        { { RatingsModifierType::NoModifier, 0, 0, 0, 0 } }
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::shopOrStall,
};
} // namespace OpenRCT2
// clang-format on
