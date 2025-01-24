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
constexpr RideTypeDescriptor SplashBoatsRTD =
{
    .Category = RIDE_CATEGORY_WATER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::splashBoats,
        .supportType = WoodenSupportType::Truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::slopeSteepDown, TrackGroup::sBend, TrackGroup::curve, TrackGroup::onridePhoto},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                     RtdFlag::canSynchroniseWithAdjacentStations, RtdFlag::hasDataLogging, RtdFlag::hasDrops,
                     RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain, RtdFlag::hasVehicleColours,
                     RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic,
                     RtdFlag::hasEntranceAndExit, RtdFlag::allowMoreVehiclesThanStationFits, RtdFlag::hasAirTime,
                     RtdFlag::showInTrackDesigner, RtdFlag::slightlyInterestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_SPLASH_BOATS, STR_RIDE_DESCRIPTION_SPLASH_BOATS },
    .NameConvention = { RideComponentType::Boat, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_CONTROL_FAILURE),
    .Heights = { 16, 24, 7, 11, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftFlume, 5, 5 },
    .RatingsMultipliers = { 80, 34, 6 },
    .UpkeepCosts = { 70, 20, 0, 9, 0, 10 },
    .BuildCosts = { 28.50_GBP, 2.50_GBP, 30, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectWater,
    .PhotoItem = ShopItem::Photo4,
    .BonusValue = 65,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BLACK },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_GREY },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SPLASH_BOATS_TRACK, SPR_RIDE_DESIGN_PREVIEW_SPLASH_BOATS_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "splash_boats",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(1, 46), MakeRideRating(0, 35), MakeRideRating(0, 30) },
        15,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           2000,             7208, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 05), 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                797059, 655360, 301111 },
            { RatingsModifierType::BonusDuration,         500,              13107, 0, 0 },
            { RatingsModifierType::BonusTurns,            0,                22291, 20860, 4574 },
            { RatingsModifierType::BonusDrops,            0,                87381, 93622, 62259 },
            { RatingsModifierType::BonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::BonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 6,                2, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_BoatHire,
};
// clang-format on
