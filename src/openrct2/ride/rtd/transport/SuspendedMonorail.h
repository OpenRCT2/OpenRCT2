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
constexpr RideTypeDescriptor SuspendedMonorailRTD =
{
    .Category = RIDE_CATEGORY_TRANSPORT,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::suspendedMonorail,
        .supportType = MetalSupportType::Boxed,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::canSynchroniseWithAdjacentStations,
                     RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                     RtdFlag::hasDataLogging, RtdFlag::hasLoadOptions, RtdFlag::hasVehicleColours,
                     RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::allowMoreVehiclesThanStationFits,
                     RtdFlag::allowMultipleCircuits, RtdFlag::isTransportRide, RtdFlag::showInTrackDesigner,
                     RtdFlag::isSuspended),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::Shuttle),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_SUSPENDED_MONORAIL, STR_RIDE_DESCRIPTION_SUSPENDED_MONORAIL },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_DOORS_STUCK_CLOSED) | (1 << BREAKDOWN_DOORS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 12, 40, 32, 8, },
    .MaxMass = 78,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 6, -10 },
    .UpkeepCosts = { 70, 20, 0, 10, 3, 10 },
    .BuildCosts = { 32.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = MUSIC_OBJECT_SUMMER,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 60,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BLACK, COLOUR_BLACK },
        { COLOUR_DARK_PURPLE, COLOUR_DARK_PURPLE, COLOUR_BLACK },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_MONORAIL_TRACK, SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_MONORAIL_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "suspended_monorail",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 15), RIDE_RATING(0, 23), RIDE_RATING(0, 8) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,            6000,             764, 0, 0 },
            { RatingsModifierType::BonusTrainLength,       0,                93622, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,          0,                44281, 70849, 35424 },
            { RatingsModifierType::BonusAverageSpeed,      0,                291271, 218453, 0 },
            { RatingsModifierType::BonusDuration,          150,              21845, 0, 0 },
            { RatingsModifierType::BonusSheltered,         0,                5140, 6553, 18724 },
            { RatingsModifierType::BonusProximity,         0,                12525, 0, 0 },
            { RatingsModifierType::BonusScenery,           0,                25098, 0, 0 },
            { RatingsModifierType::RequirementLength,      0xAA0000,         2, 2, 2 },
            { RatingsModifierType::RequirementUnsheltered, 4,                4, 1, 1 },
        },
    },
};
// clang-format on
