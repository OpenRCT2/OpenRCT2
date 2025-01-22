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
constexpr RideTypeDescriptor MonorailRTD =
{
    .Category = RIDE_CATEGORY_TRANSPORT,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::monorail,
        .supportType = MetalSupportType::Boxed,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourAdditional, RtdFlag::canSynchroniseWithAdjacentStations,
                     RtdFlag::hasTrackColourSupports, RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                     RtdFlag::hasDataLogging, RtdFlag::hasLoadOptions, RtdFlag::hasVehicleColours,
                     RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::allowMoreVehiclesThanStationFits,
                     RtdFlag::allowMultipleCircuits, RtdFlag::isTransportRide, RtdFlag::showInTrackDesigner),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::Shuttle),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_MONORAIL, STR_RIDE_DESCRIPTION_MONORAIL },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_DOORS_STUCK_CLOSED) | (1 << BREAKDOWN_DOORS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 8, 32, 8, 9, },
    .MaxMass = 78,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 6, -10 },
    .UpkeepCosts = { 65, 20, 0, 10, 3, 10 },
    .BuildCosts = { 21.00_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 60,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_GREY, COLOUR_GREY, COLOUR_DARK_OLIVE_GREEN },
        { COLOUR_GREY, COLOUR_GREY, COLOUR_SATURATED_BROWN },
        { COLOUR_GREY, COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_WHITE },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MONORAIL_TRACK, SPR_RIDE_DESIGN_PREVIEW_MONORAIL_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "monorail",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 00), MakeRideRating(0, 00), MakeRideRating(0, 00) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,            6000,     764, 0, 0 },
            { RatingsModifierType::BonusTrainLength,       0,        93622, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,          0,        44281, 70849, 35424 },
            { RatingsModifierType::BonusAverageSpeed,      0,        291271, 218453, 0 },
            { RatingsModifierType::BonusDuration,          150,      21845, 0, 0 },
            { RatingsModifierType::BonusSheltered,         0,        5140, 6553, 18724 },
            { RatingsModifierType::BonusProximity,         0,        8946, 0, 0 },
            { RatingsModifierType::BonusScenery,           0,        16732, 0, 0 },
            { RatingsModifierType::RequirementLength,      0xAA0000, 2, 2, 2 },
            { RatingsModifierType::RequirementUnsheltered, 4,        4, 1, 1 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_Monorail,
};
// clang-format on
