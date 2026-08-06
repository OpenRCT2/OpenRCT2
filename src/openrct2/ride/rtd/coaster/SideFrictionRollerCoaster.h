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
#include "../../ShopItem.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor SideFrictionRollerCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::sideFrictionRollerCoaster,
        .supportType = WoodenSupportType::truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::slope, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::brakes, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .Naming = { STR_RIDE_NAME_SIDE_FRICTION_ROLLER_COASTER, STR_RIDE_DESCRIPTION_SIDE_FRICTION_ROLLER_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 18, 24, 4, 11, },
    .MaxMass = 15,
    .LiftData = { Audio::SoundId::liftWood, 3, 5 },
    .RatingsMultipliers = { 48, 28, 7 },
    .UpkeepCosts = { 39, 20, 80, 10, 3, 10 },
    .BuildCosts = { 27.50_GBP, 3.00_GBP, 37, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::photo3,
    .BonusValue = 65,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::white, Drawing::Colour::darkBrown, Drawing::Colour::white },
        { Drawing::Colour::saturatedBrown, Drawing::Colour::white, Drawing::Colour::saturatedBrown },
        { Drawing::Colour::darkBrown, Drawing::Colour::lightBrown, Drawing::Colour::darkBrown },
        { Drawing::Colour::white, Drawing::Colour::viridian, Drawing::Colour::white }, // Leap-The-Dips
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SIDE_FRICTION_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_SIDE_FRICTION_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "side_friction_rc",
    .RatingsData = 
    {
        RatingsCalculationType::normal,
        { RideRating::make(2, 50), RideRating::make(2, 00), RideRating::make(1, 50) },
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
            { RatingsModifierType::bonusGForces,          0,                28672, 35746, 49648 },
            { RatingsModifierType::bonusTurns,            0,                26749, 43458, 45749 },
            { RatingsModifierType::bonusDrops,            0,                40777, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::bonusReversedTrains,   0,                2, 10, 12 },
            { RatingsModifierType::bonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 6,                2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0x50000,          2, 2, 2 },
            { RatingsModifierType::requirementLength,     0xFA0000,         2, 2, 2 },
            { RatingsModifierType::requirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                28672, 35746, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
