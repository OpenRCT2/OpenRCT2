/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../SpriteIds.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor CircusRTD =
{
    .Category = RideCategory::gentle,
    .StartTrackPiece = TrackElemType::flatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::circus,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps,
                     RtdFlag::hasLoadOptions, RtdFlag::vehicleIsIntegral, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::describeAsInside, RtdFlag::hasVehicleColours,
                     RtdFlag::hasMusicByDefault, RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::circus),
    .DefaultMode = RideMode::circus,
    .Naming = { STR_RIDE_NAME_CIRCUS, STR_RIDE_DESCRIPTION_CIRCUS },
    .NameConvention = { RideComponentType::Building, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 12, 128, 3, 2, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 20, 10, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 62.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = {},
    .PhotoItem = ShopItem::photo,
    .BonusValue = 39,
    .ColourPresets = kDefaultFlatRideColourPreset,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "circus",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(2, 10), RideRating::make(0, 30), RideRating::make(0, 0) },
        9,
        7,
        false,
        { { RatingsModifierType::NoModifier, 0, 0, 0, 0 } }
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = RideAudio::CircusStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = CircusMusicUpdate,
    .Classification = RideClassification::ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = nullptr,
    .UpdateMeasurementsSpecialElements = RideUpdateMeasurementsSpecialElements_Default,
    .MusicTrackOffsetLength = RideAudio::RideMusicGetTrackOffsetLength_Circus,
};
} // namespace OpenRCT2
// clang-format on
