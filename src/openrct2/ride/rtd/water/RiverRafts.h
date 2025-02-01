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
constexpr RideTypeDescriptor RiverRaftsRTD =
{
    .Category = RIDE_CATEGORY_WATER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::splashBoats,
        .supportType = WoodenSupportType::Truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::sBend, TrackGroup::curve},
        .extraTrackGroups = {TrackGroup::slope, TrackGroup::slopeSteepDown, TrackGroup::onridePhoto},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                     RtdFlag::canSynchroniseWithAdjacentStations, RtdFlag::hasDataLogging,
                     RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain, RtdFlag::hasVehicleColours,
                     RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic,
                     RtdFlag::hasEntranceAndExit, RtdFlag::allowMoreVehiclesThanStationFits,
                     RtdFlag::showInTrackDesigner, RtdFlag::slightlyInterestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_RIVER_RAFTS, STR_RIDE_DESCRIPTION_RIVER_RAFTS },
    .NameConvention = { RideComponentType::Boat, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 12, 24, 7, 11, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 80, 34, 6 },
    .UpkeepCosts = { 50, 20, 0, 9, 0, 10 },
    .BuildCosts = { 20.00_GBP, 2.50_GBP, 30, },
    .DefaultPrices = { 10, 20 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::Photo4,
    .BonusValue = 65,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_SATURATED_BROWN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_RIVER_RAFTS_TRACK, SPR_RIDE_DESIGN_PREVIEW_RIVER_RAFTS_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "river_rafts",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(1, 45), MakeRideRating(0, 25), MakeRideRating(0, 34) },
        12,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           2000,             7208, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 05), 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                531372, 655360, 301111 },
            { RatingsModifierType::BonusDuration,         500,              13107, 0, 0 },
            { RatingsModifierType::BonusTurns,            0,                22291, 20860, 4574 },
            { RatingsModifierType::BonusDrops,            0,                78643, 93622, 62259 },
            { RatingsModifierType::BonusProximity,        0,                13420, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
        },
    },
};
// clang-format on
