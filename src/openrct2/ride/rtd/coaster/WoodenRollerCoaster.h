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
constexpr RideTypeDescriptor WoodenRollerCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::woodenRollerCoaster,
        .supportType = WoodenSupportType::truss,
        .enabledTrackGroups = {TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::verticalLoop, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::waterSplash, TrackGroup::slopeCurveBanked, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::slopeSteepLong, TrackGroup::halfLoopMedium, TrackGroup::halfLoopLarge, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown},
        .extraTrackGroups = {TrackGroup::booster},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, 
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned, RideMode::reverseInclineLaunchedShuttle),
    .DefaultMode = RideMode::continuousCircuit,
    .BoosterSettings = { 0, 68 },
    .LegacyBoosterSettings = { 0, 68 },
    .Naming = { STR_RIDE_NAME_WOODEN_ROLLER_COASTER, STR_RIDE_DESCRIPTION_WOODEN_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 41, 24, 8, 11, },
    .MaxMass = 19,
    .LiftData = { Audio::SoundId::liftWood, 5, 7 },
    .RatingsMultipliers = { 52, 33, 8 },
    .UpkeepCosts = { 40, 20, 80, 10, 3, 10 },
    .BuildCosts = { 37.50_GBP, 3.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectWildWest,
    .PhotoItem = ShopItem::photo3,
    .BonusValue = 105,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::bordeauxRed, Drawing::Colour::black, Drawing::Colour::white },
        { Drawing::Colour::brightRed, Drawing::Colour::black, Drawing::Colour::grey },
        { Drawing::Colour::yellow, Drawing::Colour::darkBrown, Drawing::Colour::darkBrown },
        { Drawing::Colour::darkWater, Drawing::Colour::bordeauxRed, Drawing::Colour::white },
        { Drawing::Colour::lightBlue, Drawing::Colour::black, Drawing::Colour::black },
        { Drawing::Colour::lightBlue, Drawing::Colour::black, Drawing::Colour::darkBrown },
        { Drawing::Colour::tan, Drawing::Colour::sepia, Drawing::Colour::lightBrown }, // Generic GCI 
        { Drawing::Colour::sepia, Drawing::Colour::umber, Drawing::Colour::grey }, // The Voyage /* Change supports to "minetrain" if possible in the future. */
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_WOODEN_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_WOODEN_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "wooden_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(3, 20), RideRating::make(2, 60), RideRating::make(2, 00) },
        19,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             873, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                364088, 655360, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                40960, 34555, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 43458, 45749 },
            { RatingsModifierType::BonusDrops,            0,                40777, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 10, 15 },
            { RatingsModifierType::BonusProximity,        0,                22367, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 12,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 10), 2, 2, 2 },
            { RatingsModifierType::RequirementLength,     0x1720000,        2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                40960, 34555, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
