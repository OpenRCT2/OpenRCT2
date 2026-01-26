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
constexpr RideTypeDescriptor CompactInvertedCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::compactInvertedCoaster,
        .supportType = MetalSupportType::tubesInverted,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::verticalLoop, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::twist, TrackGroup::halfLoop, TrackGroup::corkscrew, TrackGroup::helixDownBankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::blockBrakes, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown },
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, 
                     RtdFlag::allowMultipleCircuits, RtdFlag::isSuspended, RtdFlag::reverseInclineLaunchAffectsReliability),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned, RideMode::reverseInclineLaunchedShuttle),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 7, 27 },
    .Naming = { STR_RIDE_NAME_COMPACT_INVERTED_COASTER, STR_RIDE_DESCRIPTION_COMPACT_INVERTED_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 27, 40, 29, 8, },
    .MaxMass = 18,
    .LiftData = { Audio::SoundId::liftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 13, 3, 10 },
    .BuildCosts = { 50.00_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::photo2,
    .BonusValue = 80,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::brightYellow, Drawing::Colour::brightYellow, Drawing::Colour::brightRed },
        { Drawing::Colour::icyBlue, Drawing::Colour::icyBlue, Drawing::Colour::darkGreen },
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::brightYellow },
        { Drawing::Colour::white, Drawing::Colour::white, Drawing::Colour::darkGreen },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_COMPACT_INVERTED_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_COMPACT_INVERTED_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "compact_inverted_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(3, 15), RideRating::make(2, 80), RideRating::make(3, 20) },
        21,
        kDynamicRideShelterRating,
        true,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 42), RideRating::make(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                24576, 30980, 55606 },
            { RatingsModifierType::BonusTurns,            0,                26749, 29552, 57186 },
            { RatingsModifierType::BonusDrops,            0,                29127, 39009, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 15291, 35108 },
            { RatingsModifierType::BonusProximity,        0,                15657, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                8366, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 12,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 30), 2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                24576, 30980, 55606 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
