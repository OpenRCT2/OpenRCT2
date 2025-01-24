/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor RiverRapidsRTD =
{
    .Category = RIDE_CATEGORY_WATER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::riverRapids,
        .supportType = WoodenSupportType::Truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::curveVerySmall, TrackGroup::onridePhoto, TrackGroup::rapids, TrackGroup::waterfall, TrackGroup::whirlpool},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports,
                     RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                     RtdFlag::canSynchroniseWithAdjacentStations, RtdFlag::hasDataLogging, RtdFlag::hasDrops,
                     RtdFlag::hasLoadOptions, RtdFlag::noWallsAroundTrack, RtdFlag::guestsWillRideAgain,
                     RtdFlag::hasVehicleColours, RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::allowMoreVehiclesThanStationFits,
                     RtdFlag::hasAirTime, RtdFlag::showInTrackDesigner, RtdFlag::interestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_RIVER_RAPIDS, STR_RIDE_DESCRIPTION_RIVER_RAPIDS },
    .NameConvention = { RideComponentType::Boat, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_CONTROL_FAILURE),
    .Heights = { 9, 32, 14, 15, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftFlume, 5, 5 },
    .RatingsMultipliers = { 72, 26, 6 },
    .UpkeepCosts = { 82, 20, 0, 10, 0, 10 },
    .BuildCosts = { 31.00_GBP, 3.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectWater,
    .PhotoItem = ShopItem::Photo4,
    .BonusValue = 70,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_BLACK, COLOUR_DARK_BROWN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_RIVER_RAPIDS_TRACK, SPR_RIDE_DESIGN_PREVIEW_RIVER_RAPIDS_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "river_rapids",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(1, 20), MakeRideRating(0, 70), MakeRideRating(0, 50) },
        16,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           2000,             6225, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 30), MakeRideRating(0, 05), 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                115130, 159411, 106274 },
            { RatingsModifierType::BonusDuration,         500,              13107, 0, 0 },
            { RatingsModifierType::BonusTurns,            0,                29721, 22598, 5718 },
            { RatingsModifierType::BonusDrops,            0,                40777, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::BonusProximity,        0,                31314, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                13943, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 2,                2, 2, 2 },
            { RatingsModifierType::RequirementLength,     0xC80000,         2, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
};
// clang-format on
