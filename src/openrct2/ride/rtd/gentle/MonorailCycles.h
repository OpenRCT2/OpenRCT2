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
constexpr RideTypeDescriptor MonorailCyclesRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::monorailCycles,
        .supportType = MetalSupportType::Stick,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::canSynchroniseWithAdjacentStations,
                     RtdFlag::hasDataLogging, RtdFlag::noTestMode, RtdFlag::hasLoadOptions,
                     RtdFlag::guestsWillRideAgain, RtdFlag::hasVehicleColours, RtdFlag::hasTrack,
                     RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::slightlyInterestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_MONORAIL_CYCLES, STR_RIDE_DESCRIPTION_MONORAIL_CYCLES },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 5, 24, 8, 7, },
    .MaxMass = 2,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 10, 10 },
    .UpkeepCosts = { 47, 20, 0, 8, 3, 5 },
    .BuildCosts = { 10.00_GBP, 2.50_GBP, 30, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 45,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_GREY, COLOUR_DARK_BROWN },
        { COLOUR_GREY, COLOUR_BLACK, COLOUR_GREY },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MONORAIL_CYCLES_TRACK, SPR_RIDE_DESIGN_PREVIEW_MONORAIL_CYCLES_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "monorail_cycles",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(1, 40), MakeRideRating(0, 20), MakeRideRating(0, 00) },
        4,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 15), MakeRideRating(0, 00), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusTurns,            0,                14860, 0, 4574 },
            { RatingsModifierType::BonusDrops,            0,                8738, 0, 0 },
            { RatingsModifierType::BonusSheltered,        0,                5140, 6553, 2340 },
            { RatingsModifierType::BonusProximity,        0,                8946, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::RequirementLength,     0x8C0000,         2, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_BoatHire,
};
// clang-format on
