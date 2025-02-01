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
constexpr RideTypeDescriptor MotionSimulatorRTD =
{
    .Category = RIDE_CATEGORY_THRILL,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack2x2,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::motionSimulator,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps,
                     RtdFlag::hasLoadOptions, RtdFlag::vehicleIsIntegral, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::hasVehicleColours, RtdFlag::allowMusic,
                     RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::slightlyInterestingToLookAt, RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::FilmAvengingAviators, RideMode::FilmThrillRiders),
    .DefaultMode = RideMode::FilmAvengingAviators,
    .Naming = { STR_RIDE_NAME_MOTION_SIMULATOR, STR_RIDE_DESCRIPTION_MOTION_SIMULATOR },
    .NameConvention = { RideComponentType::Car, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 12, 64, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 24, 20, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 110.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 20, 0 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 45,
    .ColourPresets = kDefaultFlatRideColourPreset,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "motion_simulator",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { 1, 1, 1 },
        21,
        7,
        false,
        {
            { RatingsModifierType::BonusMotionSimulatorMode, 0, 0, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = nullptr,
    .UpdateMeasurementsSpecialElements = RideUpdateMeasurementsSpecialElements_Default,
    .MusicTrackOffsetLength = OpenRCT2::RideAudio::RideMusicGetTrackOffsetLength_Default,
    .UpdateRideApproachVehicleWaypoints = UpdateRideApproachVehicleWaypointsMotionSimulator,
};
// clang-format on
