/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor JuniorRollerCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::juniorRollerCoaster,
        .supportType = MetalSupportType::Fork,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill,TrackGroup::liftHillCurve, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::flatToSteepSlope, TrackGroup::slopeCurve, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::blockBrakes, TrackGroup::booster, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes},
        .extraTrackGroups = {TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::onridePhoto},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::allowDoorsOnTrack,
                     RtdFlag::checkGForces, RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .TrackSpeedSettings = { 30, 15 },
    .BoosterSettings = { 17, 16 },
    .LegacyBoosterSettings = { 17, 16, 1 },
    .Naming = { STR_RIDE_NAME_JUNIOR_ROLLER_COASTER, STR_RIDE_DESCRIPTION_JUNIOR_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 12, 24, 4, 7, },
    .MaxMass = 18,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftFrictionWheels, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 8, 3, 5 },
    .BuildCosts = { 22.50_GBP, 2.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_SUMMER,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 60,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_SATURATED_BROWN },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_DARK_OLIVE_GREEN },
        { COLOUR_TEAL, COLOUR_YELLOW, COLOUR_TEAL },
        { COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN, COLOUR_OLIVE_GREEN },
        { COLOUR_BLACK, COLOUR_LIGHT_BLUE, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_JUNIOR_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_JUNIOR_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "junior_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 40), RIDE_RATING(2, 50), RIDE_RATING(1, 80) },
        13,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 40), RIDE_RATING(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                20480, 23831, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                25700, 30583, 35108 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 10, 12 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                9760, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 6,                2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0x70000,          2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   1,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                20480, 23831, 49648 },
        },
    },
};
// clang-format on
