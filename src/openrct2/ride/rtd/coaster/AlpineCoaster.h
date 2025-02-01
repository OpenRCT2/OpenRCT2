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
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor AlpineCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::alpineCoaster,
        .supportType = MetalSupportType::Fork,
        .enabledTrackGroups = { TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::flatToSteepSlope, TrackGroup::slopeCurve, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf},
        .extraTrackGroups =  {TrackGroup::helixUpBankedHalf},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports,
                          RtdFlag::hasDataLogging, RtdFlag::hasDrops, RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain,
                          RtdFlag::hasVehicleColours, RtdFlag::checkForStalling, RtdFlag::hasTrack,
                          RtdFlag::allowMusic, RtdFlag::interestingToLookAt, RtdFlag::supportsMultipleColourSchemes,
                          RtdFlag::canSynchroniseWithAdjacentStations, RtdFlag::hasEntranceAndExit,
                          RtdFlag::noTestMode, RtdFlag::allowMoreVehiclesThanStationFits, RtdFlag::upInclineRequiresLift),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .TrackSpeedSettings = { 10, 10 },
    .Naming = { STR_RIDE_NAME_ALPINE_COASTER, STR_RIDE_DESCRIPTION_ALPINE_COASTER },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = {18, 24, 3, 7},
    .MaxMass = 4,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 4, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 35, 20, 80, 8, 3, 10 },
    .BuildCosts = { 22.50_GBP, 2.50_GBP, 50},
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 65,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_ALPINE_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_ALPINE_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "alpine_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 30), MakeRideRating(2, 10), MakeRideRating(1, 4) },
        7,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             873, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 75), MakeRideRating(0, 5), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         300,              26214, 0, 0 },
            { RatingsModifierType::BonusTurns,            0,                29721, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                8738, 5461, 6553 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::RequirementMaxSpeed,   0x50000,          2, 2, 2 },
            { RatingsModifierType::RequirementLength,     0x1720000,        2, 2, 2 },
        },
    },
};
// clang-format on
