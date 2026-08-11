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
#include "../../RideStringIds.h"
#include "../../ShopItem.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor kLogFlumeRTD =
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
    .flags = kRtdFlagsHasThreeColours | RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                     RtdFlag::canSynchroniseWithAdjacentStations, RtdFlag::hasDataLogging, RtdFlag::hasDrops,
                     RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain, RtdFlag::hasVehicleColours, RtdFlag::hasTrack,
                     RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::allowMoreVehiclesThanStationFits, RtdFlag::hasAirTime, RtdFlag::showInTrackDesigner,
                     RtdFlag::interestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 5, 27 },
    .Naming = { STR_RIDE_NAME_LOG_FLUME, STR_RIDE_DESCRIPTION_LOG_FLUME },
    .NameConvention = { RideComponentType::boat, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::brakesFailure },
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
    .ColourKey = RideColourKey::ride,
    .Name = "log_flume",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(1, 50), RideRating::make(0, 55), RideRating::make(0, 30) },
        15,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           2000,             7208, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                531372, 655360, 301111 },
            { RatingsModifierType::bonusDuration,         300,              13107, 0, 0 },
            { RatingsModifierType::bonusTurns,            0,                22291, 20860, 4574 },
            { RatingsModifierType::bonusDrops,            0,                69905, 62415, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::bonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 6,                2, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::standard,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAdjustment_LogFlume,
};
} // namespace OpenRCT2
// clang-format on
