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
constexpr RideTypeDescriptor LogFlumeRTD =
{
    .Category = RideCategory::water,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::logFlume,
        .supportType = MetalSupportType::boxed,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::onridePhoto, TrackGroup::logFlumeReverser, TrackGroup::slopeSteepDown},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                     RtdFlag::canSynchroniseWithAdjacentStations, RtdFlag::hasDataLogging, RtdFlag::hasDrops,
                     RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain, RtdFlag::hasVehicleColours, RtdFlag::hasTrack,
                     RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::allowMoreVehiclesThanStationFits, RtdFlag::hasAirTime, RtdFlag::showInTrackDesigner,
                     RtdFlag::interestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .Naming = { STR_RIDE_NAME_LOG_FLUME, STR_RIDE_DESCRIPTION_LOG_FLUME },
    .NameConvention = { RideComponentType::Boat, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 10, 24, 7, 9, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::liftFlume, 5, 5 },
    .RatingsMultipliers = { 80, 34, 6 },
    .UpkeepCosts = { 80, 20, 0, 9, 0, 10 },
    .BuildCosts = { 22.50_GBP, 2.50_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectWater,
    .PhotoItem = ShopItem::photo4,
    .BonusValue = 65,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::darkBrown, Drawing::Colour::darkBrown, Drawing::Colour::grey },
        { Drawing::Colour::oliveGreen, Drawing::Colour::black, Drawing::Colour::grey },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_LOG_FLUME_TRACK, SPR_RIDE_DESIGN_PREVIEW_LOG_FLUME_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "log_flume",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(1, 50), RideRating::make(0, 55), RideRating::make(0, 30) },
        15,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,           2000,             7208, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                531372, 655360, 301111 },
            { RatingsModifierType::BonusDuration,         300,              13107, 0, 0 },
            { RatingsModifierType::BonusTurns,            0,                22291, 20860, 4574 },
            { RatingsModifierType::BonusDrops,            0,                69905, 62415, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::BonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 6,                2, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_LogFlume,
};
} // namespace OpenRCT2
// clang-format on
