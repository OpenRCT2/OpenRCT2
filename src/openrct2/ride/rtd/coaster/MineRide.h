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
#include "../../ShopItem.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor kMineRideRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::mineRide,
        .supportType = MetalSupportType::fork,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::onridePhoto, TrackGroup::diagSlope, TrackGroup::helixUpUnbankedQuarter, TrackGroup::helixDownUnbankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::helixDownBankedQuarter },
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 7, 27 },
    .Naming = { STR_RIDE_NAME_MINE_RIDE, STR_RIDE_DESCRIPTION_MINE_RIDE },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction },
    .Heights = { 13, 24, 9, 11, },
    .MaxMass = 27,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 60, 20, 10 },
    .UpkeepCosts = { 50, 20, 80, 10, 3, 10 },
    .BuildCosts = { 42.50_GBP, 2.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectWildWest,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 70,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::darkBrown, Drawing::Colour::darkBrown, Drawing::Colour::black },
        { Drawing::Colour::bordeauxRed, Drawing::Colour::bordeauxRed, Drawing::Colour::darkBrown },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MINE_RIDE_TRACK, SPR_RIDE_DESIGN_PREVIEW_MINE_RIDE_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "mine_ride",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(2, 75), RideRating::make(1, 00), RideRating::make(1, 80) },
        16,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                40960, 29789, 49648 },
            { RatingsModifierType::bonusTurns,            0,                29721, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                19275, 32768, 35108 },
            { RatingsModifierType::bonusReversedTrains,   0,                2, 10, 12 },
            { RatingsModifierType::bonusProximity,        0,                21472, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                16732, 0, 0 },
            { RatingsModifierType::requirementLength,     0x10E0000,        2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                40960, 29789, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
