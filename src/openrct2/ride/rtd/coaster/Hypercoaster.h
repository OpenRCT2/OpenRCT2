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
constexpr RideTypeDescriptor HypercoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::corkscrewRollerCoaster,
        .supportType = MetalSupportType::tubes,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::blockBrakes, TrackGroup::slopeSteepLong, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::slopeRollBanking, TrackGroup::slopeCurveBanked, TrackGroup::slopeCurveLarge, TrackGroup::diagSlopeSteepLong, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown },
        .extraTrackGroups = { TrackGroup::verticalLoop, TrackGroup::halfLoop, TrackGroup::corkscrew, TrackGroup::booster, TrackGroup::corkscrewLarge, TrackGroup::halfLoopMedium, TrackGroup::halfLoopLarge, TrackGroup::twist, TrackGroup::slopeVertical, TrackGroup::curveVertical, TrackGroup::quarterLoop, TrackGroup::barrelRoll, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::diveLoop },
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                     RtdFlag::checkGForces, RtdFlag::allowMultipleCircuits, RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned, RideMode::poweredLaunchPasstrough, RideMode::poweredLaunch, RideMode::reverseInclineLaunchedShuttle),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .TrackSpeedSettings = { 60, 60 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_HYPERCOASTER, STR_RIDE_DESCRIPTION_HYPERCOASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 55, 24, 8, 11, },
    .MaxMass = 18,
    .LiftData = { Audio::SoundId::liftArrow, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 11, 3, 10 },
    .BuildCosts = { 52.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 100,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::bordeauxRed, Drawing::Colour::bordeauxRed, Drawing::Colour::black },
        { Drawing::Colour::lightBlue, Drawing::Colour::lightBlue, Drawing::Colour::white },
        { Drawing::Colour::black, Drawing::Colour::bordeauxRed, Drawing::Colour::black },
        { Drawing::Colour::white, Drawing::Colour::white, Drawing::Colour::darkGreen },
        { Drawing::Colour::seafoamGreen, Drawing::Colour::seafoamGreen, Drawing::Colour::black }, // Phantom's Revenge (Old)
        { Drawing::Colour::violet, Drawing::Colour::violet, Drawing::Colour::black }, // Phantom's Revenge (New)
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_CORKSCREW_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_CORKSCREW_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "hypercoaster",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(3, 00), RideRating::make(0, 50), RideRating::make(0, 20) },
        16,
        kDynamicRideShelterRating,
        true,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                24576, 35746, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 20, 30 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 12,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 40), 2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   2,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 35746, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
