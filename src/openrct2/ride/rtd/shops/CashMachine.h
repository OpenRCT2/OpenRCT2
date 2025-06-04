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
constexpr RideTypeDescriptor CashMachineRTD =
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
                     RtdFlag::noVehicles, RtdFlag::isShopOrFacility,
                     RtdFlag::noWallsAroundTrack, RtdFlag::isFlatRide, RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::shopStall),
    .DefaultMode = RideMode::shopStall,
    .Naming = { STR_RIDE_NAME_CASH_MACHINE, STR_RIDE_DESCRIPTION_CASH_MACHINE },
    .NameConvention = { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station },
    .AvailableBreakdowns = 0,
    .Heights = { 12, kDefaultCashMachineHeight, 0, 0, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 0, 0, 0 },
    .UpkeepCosts = { 40, 1, 0, 0, 0, 0 },
    .BuildCosts = { 200.00_GBP, 0.00_GBP, 1, },
    .DefaultPrices = { 0, 0 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 5,
    .ColourPresets = kDefaultFlatRideColourPreset,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::CashMachine,
    .Name = "cash_machine",
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
    .Classification = RideClassification::kioskOrFacility,
    .specialType = RtdSpecialType::cashMachine,
};
} // namespace OpenRCT2
// clang-format on
