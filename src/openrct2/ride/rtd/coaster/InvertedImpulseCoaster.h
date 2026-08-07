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
constexpr RideTypeDescriptor InvertedImpulseCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::invertedImpulseCoaster,
        .supportType = MetalSupportType::tubesInverted,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeVertical, TrackGroup::curveVertical},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
                 RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                              RtdFlag::allowMultipleCircuits, RtdFlag::isSuspended, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::poweredLaunchPassthrough, RideMode::poweredLaunch),
    .DefaultMode = RideMode::poweredLaunchPassthrough,
    .OperatingSettings = { 10, 33 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_INVERTED_IMPULSE_COASTER, STR_RIDE_DESCRIPTION_INVERTED_IMPULSE_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction },
    .Heights = { 45, 40, 29, 8, },
    .MaxMass = 23,
    .LiftData = { Audio::SoundId::liftClassic, 4, 7 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 180, 20, 80, 11, 3, 10 },
    .BuildCosts = { 62.50_GBP, 2.50_GBP, 25, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo2,
    .BonusValue = 75,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::grey },
        { Drawing::Colour::yellow, Drawing::Colour::yellow, Drawing::Colour::white },
        { Drawing::Colour::brightPurple, Drawing::Colour::brightPurple, Drawing::Colour::bordeauxRed },
        { Drawing::Colour::brightYellow, Drawing::Colour::brightYellow, Drawing::Colour::bordeauxRed }, // Volcano: The Blast Coaster
        { Drawing::Colour::brightYellow, Drawing::Colour::brightYellow, Drawing::Colour::darkWater }, // Wicked Twister
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_INVERTED_IMPULSE_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_INVERTED_IMPULSE_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "inverted_impulse_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(4, 00), RideRating::make(3, 00), RideRating::make(3, 20) },
        20,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 42), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                24576, 29789, 55606 },
            { RatingsModifierType::bonusTurns,            0,                26749, 29552, 57186 },
            { RatingsModifierType::bonusDrops,            0,                29127, 39009, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                15420, 15291, 35108 },
            { RatingsModifierType::bonusProximity,        0,                15657, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                9760, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 20,               2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                24576, 29789, 55606 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
