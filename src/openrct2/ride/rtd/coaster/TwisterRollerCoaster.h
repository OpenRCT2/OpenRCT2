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
#include "../../../paint/support/MetalSupports.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor TwisterRollerCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::twisterRollerCoaster,
        .supportType = MetalSupportType::tubes,
        .enabledTrackGroups = {TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::verticalLoop, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::halfLoop, TrackGroup::corkscrew, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::barrelRoll, TrackGroup::poweredLift, TrackGroup::halfLoopLarge, TrackGroup::slopeCurveBanked, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::slopeSteepLong, TrackGroup::curveVertical, TrackGroup::quarterLoop, TrackGroup::booster, TrackGroup::twist, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::slopeCurveLarge, TrackGroup::corkscrewLarge, TrackGroup::halfLoopMedium, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::diagSlopeSteepLong, TrackGroup::diveLoop, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown},
        .extraTrackGroups = {TrackGroup::liftHillSteep, TrackGroup::brakeForDrop},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, 
                     RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 17, 68 },
    .LegacyBoosterSettings = { 17, 68 },
    .Naming = { STR_RIDE_NAME_TWISTER_ROLLER_COASTER, STR_RIDE_DESCRIPTION_TWISTER_ROLLER_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 40, 24, 8, 9, },
    .MaxMass = 31,
    .LiftData = { Audio::SoundId::liftBM, 5, 8 },
    .RatingsMultipliers = { 52, 36, 10 },
    .UpkeepCosts = { 43, 20, 80, 11, 3, 10 },
    .BuildCosts = { 60.00_GBP, 2.50_GBP, 55, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 120,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::yellow, Drawing::Colour::yellow, Drawing::Colour::bordeauxRed },
        { Drawing::Colour::lightWater, Drawing::Colour::lightWater, Drawing::Colour::darkPurple },
        { Drawing::Colour::white, Drawing::Colour::white, Drawing::Colour::lightBlue },
        { Drawing::Colour::darkGreen, Drawing::Colour::mossGreen, Drawing::Colour::darkBrown },
        { Drawing::Colour::bordeauxRed, Drawing::Colour::lightOrange, Drawing::Colour::white },
        { Drawing::Colour::lightBlue, Drawing::Colour::yellow, Drawing::Colour::darkPink }, // Scream
        { Drawing::Colour::darkOrange, Drawing::Colour::darkOrange, Drawing::Colour::black }, // Rougarou
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_TWISTER_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_TWISTER_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "twister_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(3, 50), RideRating::make(0, 40), RideRating::make(0, 30) },
        15,
        kDynamicRideShelterRating,
        true,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                24576, 32768, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 12, 20 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 12,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 32768, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
