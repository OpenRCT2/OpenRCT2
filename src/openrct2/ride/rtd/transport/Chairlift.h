/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../drawing/LightFX.h"
#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor ChairliftRTD =
{
    .Category = RIDE_CATEGORY_TRANSPORT,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::chairlift,
        .supportType = MetalSupportType::Truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::curveVerySmall},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports, RtdFlag::canSynchroniseWithAdjacentStations,
                     RtdFlag::hasLoadOptions, RtdFlag::hasVehicleColours, RtdFlag::hasTrack,
                     RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::allowMoreVehiclesThanStationFits, RtdFlag::isTransportRide, RtdFlag::showInTrackDesigner,
                     RtdFlag::slightlyInterestingToLookAt, RtdFlag::isSuspended),
    .RideModes = EnumsToFlags(RideMode::StationToStation),
    .DefaultMode = RideMode::StationToStation,
    .OperatingSettings = { 1, 4 },
    .Naming = { STR_RIDE_NAME_CHAIRLIFT, STR_RIDE_DESCRIPTION_CHAIRLIFT },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 40, 32, 28, 2, },
    .MaxMass = 18,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 10, 0 },
    .UpkeepCosts = { 60, 20, 0, 4, 3, 10 },
    .BuildCosts = { 32.50_GBP, 0.50_GBP, 30, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_GREY },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_OLIVE_GREEN },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_WHITE },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_CHAIRLIFT_TRACK, SPR_RIDE_DESIGN_PREVIEW_CHAIRLIFT_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "chairlift",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(1, 60), MakeRideRating(0, 40), MakeRideRating(0, 50) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,            6000,     764, 0, 0 },
            { RatingsModifierType::BonusTrainLength,       0,        187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,          0,        44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,      0,        291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,          150,      26214, 0, 0 },
            { RatingsModifierType::BonusTurns,             0,        7430, 3476, 4574 },
            { RatingsModifierType::BonusSheltered,         0,        -19275, 21845, 23405 },
            { RatingsModifierType::BonusProximity,         0,        11183, 0, 0 },
            { RatingsModifierType::BonusScenery,           0,        25098, 0, 0 },
            { RatingsModifierType::RequirementLength,      0x960000, 2, 2, 2 },
            { RatingsModifierType::RequirementStations,    1,        0, 2, 1 },
            { RatingsModifierType::RequirementUnsheltered, 4,        4, 1, 1 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_ChairLift,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = UpdateChairlift,
};
// clang-format on
