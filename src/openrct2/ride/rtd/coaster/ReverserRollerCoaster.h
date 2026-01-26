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
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor ReverserRollerCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::reverserRollerCoaster,
        .supportType = WoodenSupportType::truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::slope, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::brakes, TrackGroup::reverser},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
                 EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                              RtdFlag::checkGForces, RtdFlag::layeredVehiclePreview),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .Naming = { STR_RIDE_NAME_REVERSER_ROLLER_COASTER, STR_RIDE_DESCRIPTION_REVERSER_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 18, 24, 8, 11, },
    .MaxMass = 15,
    .LiftData = { Audio::SoundId::liftWood, 3, 5 },
    .RatingsMultipliers = { 48, 28, 7 },
    .UpkeepCosts = { 39, 20, 80, 10, 3, 10 },
    .BuildCosts = { 27.50_GBP, 3.00_GBP, 37, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::photo3,
    .BonusValue = 65,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::darkBrown, Drawing::Colour::lightBrown, Drawing::Colour::darkBrown },
        { Drawing::Colour::saturatedBrown, Drawing::Colour::bordeauxRed, Drawing::Colour::saturatedBrown },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "reverser_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(2, 40), RideRating::make(1, 80), RideRating::make(1, 70) },
        19,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,     873, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,        RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,        187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,        44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,        364088, 655360, 0 },
            { RatingsModifierType::BonusReversals,        6,        RideRating::make(0, 20), RideRating::make(0, 20), RideRating::make(0, 20) },
            { RatingsModifierType::BonusDuration,         150,      26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,        28672, 23831, 49648 },
            { RatingsModifierType::BonusTurns,            0,        26749, 43458, 45749 },
            { RatingsModifierType::BonusDrops,            0,        40777, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,        16705, 30583, 35108 },
            { RatingsModifierType::BonusProximity,        0,        22367, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,        11155, 0, 0 },
            { RatingsModifierType::RequirementReversals,  1,        8, 1, 1 },
            { RatingsModifierType::RequirementLength,     0xC80000, 2, 1, 1 },
            { RatingsModifierType::RequirementNumDrops,   2,        2, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,        28672, 23831, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
