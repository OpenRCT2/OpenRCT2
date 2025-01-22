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
constexpr RideTypeDescriptor MiniatureRailwayRTD =
{
    .Category = RIDE_CATEGORY_TRANSPORT,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::miniatureRailway,
        .supportType = WoodenSupportType::Truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourSupports, RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::canSynchroniseWithAdjacentStations,
                     RtdFlag::hasDataLogging, RtdFlag::hasLoadOptions, RtdFlag::hasVehicleColours,
                     RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::allowMoreVehiclesThanStationFits,
                     RtdFlag::allowMultipleCircuits, RtdFlag::isTransportRide, RtdFlag::showInTrackDesigner,
                     RtdFlag::supportsLevelCrossings),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::Shuttle),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_MINIATURE_RAILWAY, STR_RIDE_DESCRIPTION_MINIATURE_RAILWAY },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 7, 32, 5, 9, },
    .MaxMass = 39,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 6, -10 },
    .UpkeepCosts = { 60, 20, 0, 10, 3, 5 },
    .BuildCosts = { 17.50_GBP, 3.00_GBP, 50, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 50,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_WHITE, COLOUR_DARK_BROWN },
        { COLOUR_BRIGHT_RED, COLOUR_WHITE, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MINIATURE_RAILWAY_TRACK, SPR_RIDE_DESIGN_PREVIEW_MINIATURE_RAILWAY_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "miniature_railway",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 50), MakeRideRating(0, 00), MakeRideRating(0, 00) },
        11,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,            6000,     764, 0, 0 },
            { RatingsModifierType::BonusTrainLength,       0,        140434, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,          0,        44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,      0,        291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,          150,      26214, 0, 0 },
            { RatingsModifierType::BonusSheltered,         0,        -6425, 6553, 23405 },
            { RatingsModifierType::BonusProximity,         0,        8946, 0, 0 },
            { RatingsModifierType::BonusScenery,           0,        20915 , 0, 0},
            { RatingsModifierType::RequirementLength,      0xC80000, 2, 2, 2 },
            { RatingsModifierType::RequirementUnsheltered, 4,        4, 1, 1 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_MiniatureRailway,
};
// clang-format on
