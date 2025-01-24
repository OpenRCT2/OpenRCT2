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
constexpr RideTypeDescriptor GhostTrainRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::ghostTrain,
        .supportType = MetalSupportType::Boxed,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::curveVerySmall, TrackGroup::curveSmall, TrackGroup::brakes, TrackGroup::spinningTunnel},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::canSynchroniseWithAdjacentStations,
                     RtdFlag::hasGForces, RtdFlag::hasDataLogging, RtdFlag::hasDrops,
                     RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain, RtdFlag::hasVehicleColours,
                     RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowDoorsOnTrack,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::allowMoreVehiclesThanStationFits,
                     RtdFlag::hasAirTime, RtdFlag::showInTrackDesigner, RtdFlag::interestingToLookAt,
                     RtdFlag::hasLandscapeDoors),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_GHOST_TRAIN, STR_RIDE_DESCRIPTION_GHOST_TRAIN },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 8, 24, 6, 7, },
    .MaxMass = 2,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 10, 10 },
    .UpkeepCosts = { 80, 20, 0, 8, 3, 5 },
    .BuildCosts = { 13.00_GBP, 2.50_GBP, 30, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectHorror,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 50,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_GREEN, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_TEAL, COLOUR_WHITE, COLOUR_DARK_BROWN },
        { COLOUR_DARK_BROWN, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_GHOST_TRAIN_TRACK, SPR_RIDE_DESIGN_PREVIEW_GHOST_TRAIN_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "ghost_train",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 00), MakeRideRating(0, 20), MakeRideRating(0, 03) },
        12,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 15), MakeRideRating(0, 00), 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusTurns,            0,                14860, 0, 11437 },
            { RatingsModifierType::BonusDrops,            0,                8738, 0, 0 },
            { RatingsModifierType::BonusSheltered,        0,                25700, 6553, 4681 },
            { RatingsModifierType::BonusProximity,        0,                11183, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                8366, 0, 0 },
            { RatingsModifierType::RequirementLength,     0xB40000,         2, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_MineTrainCoaster,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_GhostTrain,
};
// clang-format on
