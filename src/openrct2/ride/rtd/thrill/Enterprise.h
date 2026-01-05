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
constexpr RideTypeDescriptor EnterpriseRTD =
{
    .Category = RideCategory::thrill,
    .StartTrackPiece = TrackElemType::flatTrack4x4,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::enterprise,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps,
                     RtdFlag::hasLoadOptions, RtdFlag::vehicleIsIntegral, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::guestsWillRideAgain, RtdFlag::hasVehicleColours,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::interestingToLookAt, RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::rotation),
    .DefaultMode = RideMode::rotation,
    .OperatingSettings = { 10, 20 },
    .Naming = { STR_RIDE_NAME_ENTERPRISE, STR_RIDE_DESCRIPTION_ENTERPRISE },
    .NameConvention = { RideComponentType::Wheel, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 16, 160, 3, 2, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 50, 10, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 50.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 20, 0 },
    .DefaultMusic = kMusicObjectRock2,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 45,
    .ColourPresets = kDefaultFlatRideColourPreset,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "enterprise",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(3, 60), RideRating::make(4, 55), RideRating::make(5, 72) },
        22,
        3,
        false,
        {
            { RatingsModifierType::BonusOperationOption, 0, 1, 16, 16 }, 
            { RatingsModifierType::BonusScenery,         0, 19521, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingEnterprise,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationEnterprise,
    .specialType = RtdSpecialType::enterprise,
};
} // namespace OpenRCT2
// clang-format on
