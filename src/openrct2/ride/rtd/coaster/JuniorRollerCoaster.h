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
constexpr RideTypeDescriptor JuniorRollerCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::juniorRollerCoaster,
        .supportType = MetalSupportType::fork,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill,TrackGroup::liftHillCurve, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::flatToSteepSlope, TrackGroup::slopeCurve, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::blockBrakes, TrackGroup::booster, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::diagSlope },
        .extraTrackGroups = { TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::onridePhoto, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown },
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::allowDoorsOnTrack,
                     RtdFlag::checkGForces, RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 5, 18 },
    .TrackSpeedSettings = { 30, 15 },
    .BoosterSettings = { 17, 16 },
    .LegacyBoosterSettings = { 17, 16, 1 },
    .Naming = { STR_RIDE_NAME_JUNIOR_ROLLER_COASTER, STR_RIDE_DESCRIPTION_JUNIOR_ROLLER_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 12, 24, 4, 7, },
    .MaxMass = 18,
    .LiftData = { Audio::SoundId::liftFrictionWheels, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 8, 3, 5 },
    .BuildCosts = { 22.50_GBP, 2.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 60,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::yellow, Drawing::Colour::yellow, Drawing::Colour::saturatedBrown },
        { Drawing::Colour::bordeauxRed, Drawing::Colour::bordeauxRed, Drawing::Colour::darkOliveGreen },
        { Drawing::Colour::darkWater, Drawing::Colour::yellow, Drawing::Colour::darkWater },
        { Drawing::Colour::saturatedBrown, Drawing::Colour::saturatedBrown, Drawing::Colour::oliveGreen },
        { Drawing::Colour::black, Drawing::Colour::lightBlue, Drawing::Colour::black },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_JUNIOR_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_JUNIOR_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "junior_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(2, 40), RideRating::make(2, 50), RideRating::make(1, 80) },
        13,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                20480, 23831, 49648 },
            { RatingsModifierType::bonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                25700, 30583, 35108 },
            { RatingsModifierType::bonusReversedTrains,   0,                2, 10, 12 },
            { RatingsModifierType::bonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                9760, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 6,                2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0x70000,          2, 2, 2 },
            { RatingsModifierType::requirementNumDrops,   1,                2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                20480, 23831, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
