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
constexpr RideTypeDescriptor kWoodenWildMouseRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::woodenWildMouse,
        .supportType = WoodenSupportType::truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::liftHillSteep, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::flatToSteepSlope, TrackGroup::curveVerySmall, TrackGroup::curveSmall},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports,
                     RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::allowDoorsOnTrack, RtdFlag::checkGForces),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 5, 18 },
    .Naming = { STR_RIDE_NAME_WOODEN_WILD_MOUSE, STR_RIDE_DESCRIPTION_WOODEN_WILD_MOUSE },
    .NameConvention = { RideComponentType::car, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 14, 24, 4, 7, },
    .MaxMass = 4,
    .LiftData = { Audio::SoundId::liftClassic, 4, 5 },
    .RatingsMultipliers = { 50, 30, 30 },
    .UpkeepCosts = { 40, 20, 80, 9, 3, 10 },
    .BuildCosts = { 25.00_GBP, 3.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectWildWest,
    .PhotoItem = ShopItem::photo3,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::white },
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::saturatedBrown },
        { Drawing::Colour::lightBlue, Drawing::Colour::lightBlue, Drawing::Colour::saturatedBrown },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_WOODEN_WILD_MOUSE_TRACK, SPR_RIDE_DESIGN_PREVIEW_WOODEN_WILD_MOUSE_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "wooden_wild_mouse",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(2, 90), RideRating::make(2, 90), RideRating::make(2, 10) },
        14,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           6000,             873, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 8), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                364088, 655360, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                102400, 35746, 49648 },
            { RatingsModifierType::bonusTurns,            0,                29721, 43458, 45749 },
            { RatingsModifierType::bonusDrops,            0,                40777, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::bonusProximity,        0,                17893, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                5577, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 8,                2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0x70000,          2, 2, 2 },
            { RatingsModifierType::requirementNegativeGs, MakeFixed16_2dp(0, 10), 2, 2, 2 },
            { RatingsModifierType::requirementLateralGs,  MakeFixed16_2dp(1, 50), 2, 2, 2 },
            { RatingsModifierType::requirementLength,     0xAA0000,         2, 2, 2 },
            { RatingsModifierType::requirementNumDrops,   3,                2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                102400, 35746, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
