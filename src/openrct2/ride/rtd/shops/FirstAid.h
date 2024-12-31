/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
constexpr RideTypeDescriptor FirstAidRTD =
{
    .Category = RIDE_CATEGORY_SHOP,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack1x1A,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::facility,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps, RtdFlag::noTestMode,
                     RtdFlag::noVehicles, RtdFlag::isShopOrFacility, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::guestsShouldGoInsideFacility,
                     RtdFlag::describeAsInside, RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::ShopStall),
    .DefaultMode = RideMode::ShopStall,
    .OperatingSettings = { 8, 8 },
    .Naming = { STR_RIDE_NAME_FIRST_AID, STR_RIDE_DESCRIPTION_FIRST_AID },
    .NameConvention = { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station },
    .AvailableBreakdowns = 0,
    .Heights = { 12, DefaultFirstAidHeight, 0, 0, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 0, 0, 0 },
    .UpkeepCosts = { 45, 1, 0, 0, 0, 0 },
    .BuildCosts = { 250.00_GBP, 0.00_GBP, 1, },
    .DefaultPrices = { 0, 0 },
    .DefaultMusic = MUSIC_OBJECT_GENTLE,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 5,
    .ColourPresets = DEFAULT_FLAT_RIDE_COLOUR_PRESET,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::FirstAid,
    .Name = "first_aid",
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
    .specialType = RtdSpecialType::firstAid,
};
// clang-format on
