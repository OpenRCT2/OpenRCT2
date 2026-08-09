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

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor kAlpineCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::alpineCoaster,
        .supportType = MetalSupportType::fork,
        .enabledTrackGroups = { TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::flatToSteepSlope, TrackGroup::slopeCurve, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::diagSlope, TrackGroup::helixDownUnbankedQuarter, TrackGroup::helixDownBankedQuarter },
        .extraTrackGroups =  { TrackGroup::helixUpBankedHalf, TrackGroup::helixUpUnbankedQuarter, TrackGroup::helixUpBankedQuarter },
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = RtdFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports,
                          RtdFlag::hasDataLogging, RtdFlag::hasDrops, RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain,
                          RtdFlag::hasVehicleColours, RtdFlag::checkForStalling, RtdFlag::hasTrack,
                          RtdFlag::allowMusic, RtdFlag::interestingToLookAt, RtdFlag::supportsMultipleColourSchemes,
                          RtdFlag::canSynchroniseWithAdjacentStations, RtdFlag::hasEntranceAndExit,
                          RtdFlag::noTestMode, RtdFlag::allowMoreVehiclesThanStationFits, RtdFlag::upInclineRequiresLift),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 5, 18 },
    .TrackSpeedSettings = { 10, 10 },
    .Naming = { STR_RIDE_NAME_ALPINE_COASTER, STR_RIDE_DESCRIPTION_ALPINE_COASTER },
    .NameConvention = { RideComponentType::car, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::vehicleMalfunction },
    .Heights = {18, 24, 3, 7},
    .MaxMass = 4,
    .LiftData = { Audio::SoundId::null, 4, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 35, 20, 80, 8, 3, 10 },
    .BuildCosts = { 22.50_GBP, 2.50_GBP, 50},
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 65,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::grey, Drawing::Colour::grey, Drawing::Colour::black },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_ALPINE_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_ALPINE_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "alpine_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(2, 30), RideRating::make(2, 10), RideRating::make(1, 4) },
        7,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           6000,             873, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 75), RideRating::make(0, 5), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         300,              26214, 0, 0 },
            { RatingsModifierType::bonusTurns,            0,                29721, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                8738, 5461, 6553 },
            { RatingsModifierType::bonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::bonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::requirementMaxSpeed,   0x50000,          2, 2, 2 },
            { RatingsModifierType::requirementLength,     0x1720000,        2, 2, 2 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
