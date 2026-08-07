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
constexpr RideTypeDescriptor VirginiaReelRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::virginiaReel,
        .supportType = WoodenSupportType::truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::slope, TrackGroup::curveVerySmall, TrackGroup::curveSmall},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
         RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 5, 18 },
    .Naming = { STR_RIDE_NAME_VIRGINIA_REEL, STR_RIDE_DESCRIPTION_VIRGINIA_REEL },
    .NameConvention = { RideComponentType::car, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 14, 24, 6, 7, },
    .MaxMass = 15,
    .LiftData = { Audio::SoundId::liftClassic, 3, 5 },
    .RatingsMultipliers = { 30, 15, 25 },
    .UpkeepCosts = { 39, 20, 80, 10, 3, 10 },
    .BuildCosts = { 26.50_GBP, 3.00_GBP, 25, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo3,
    .BonusValue = 50,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::saturatedBrown, Drawing::Colour::grey, Drawing::Colour::saturatedBrown },
        { Drawing::Colour::black, Drawing::Colour::darkBrown, Drawing::Colour::darkBrown },
        { Drawing::Colour::darkBrown, Drawing::Colour::bordeauxRed, Drawing::Colour::darkBrown },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_VIRGINIA_REEL_TRACK, SPR_RIDE_DESIGN_PREVIEW_VIRGINIA_REEL_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "virginia_reel",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(2, 10), RideRating::make(1, 90), RideRating::make(3, 70) },
        19,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           6000,             873, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                364088, 655360, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                110592, 29789, 59578 },
            { RatingsModifierType::bonusTurns,            0,                52012, 26075, 45749 },
            { RatingsModifierType::bonusDrops,            0,                43690, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::bonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::requirementLength,     0xD20000,         2, 2, 2 },
            { RatingsModifierType::requirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                110592, 29789, 59578 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
