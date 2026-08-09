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
constexpr RideTypeDescriptor kCompactInvertedCoasterRTD =
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
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                     RtdFlag::allowMultipleCircuits, RtdFlag::isSuspended, RtdFlag::reverseInclineLaunchAffectsReliability),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned, RideMode::reverseInclineLaunchedShuttle),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 7, 27 },
    .Naming = { STR_RIDE_NAME_COMPACT_INVERTED_COASTER, STR_RIDE_DESCRIPTION_COMPACT_INVERTED_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
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
    .ColourKey = RideColourKey::ride,
    .Name = "compact_inverted_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(3, 15), RideRating::make(2, 80), RideRating::make(3, 20) },
        21,
        kDynamicRideShelterRating,
        true,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 42), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                24576, 30980, 55606 },
            { RatingsModifierType::bonusTurns,            0,                26749, 29552, 57186 },
            { RatingsModifierType::bonusDrops,            0,                29127, 39009, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                15420, 15291, 35108 },
            { RatingsModifierType::bonusProximity,        0,                15657, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                8366, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 12,               2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::requirementNegativeGs, MakeFixed16_2dp(0, 30), 2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                24576, 30980, 55606 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
