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
constexpr RideTypeDescriptor kInvertedHairpinCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::invertedHairpinCoaster,
        .supportType = MetalSupportType::tubesInverted,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::liftHillSteep, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::flatToSteepSlope, TrackGroup::slopeCurve, TrackGroup::curveVerySmall, TrackGroup::curveSmall, TrackGroup::brakes, TrackGroup::blockBrakes},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                     RtdFlag::isSuspended),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 5, 18 },
    .Naming = { STR_RIDE_NAME_INVERTED_HAIRPIN_COASTER, STR_RIDE_DESCRIPTION_INVERTED_HAIRPIN_COASTER },
    .NameConvention = { RideComponentType::car, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 16, 24, 24, 7, },
    .MaxMass = 4,
    .LiftData = { Audio::SoundId::liftWildMouse, 4, 6 },
    .RatingsMultipliers = { 50, 30, 30 },
    .UpkeepCosts = { 40, 20, 80, 9, 3, 10 },
    .BuildCosts = { 33.50_GBP, 2.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock2,
    .PhotoItem = ShopItem::photo2,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::brightYellow, Drawing::Colour::brightYellow, Drawing::Colour::brightRed },
        { Drawing::Colour::black, Drawing::Colour::icyBlue, Drawing::Colour::black },
        { Drawing::Colour::white, Drawing::Colour::white, Drawing::Colour::yellow },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_INVERTED_HAIRPIN_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_INVERTED_HAIRPIN_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "inverted_hairpin_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(3, 00), RideRating::make(2, 65), RideRating::make(2, 25) },
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
