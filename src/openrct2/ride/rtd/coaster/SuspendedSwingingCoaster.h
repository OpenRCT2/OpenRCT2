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
constexpr RideTypeDescriptor SuspendedSwingingCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::suspendedSwingingCoaster,
        .supportType = MetalSupportType::tubes,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownUnbankedQuarter, TrackGroup::helixUpUnbankedQuarter, TrackGroup::brakes, TrackGroup::blockBrakes, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, RtdFlag::isSuspended),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .Naming = { STR_RIDE_NAME_SUSPENDED_SWINGING_COASTER, STR_RIDE_DESCRIPTION_SUSPENDED_SWINGING_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 24, 40, 29, 8, },
    .MaxMass = 26,
    .LiftData = { Audio::SoundId::liftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 20, 3, 10 },
    .BuildCosts = { 47.50_GBP, 2.50_GBP, 50, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo2,
    .BonusValue = 90,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::oliveGreen, Drawing::Colour::oliveGreen, Drawing::Colour::lightBrown },
        { Drawing::Colour::black, Drawing::Colour::lightBlue, Drawing::Colour::black },
        { Drawing::Colour::saturatedBrown, Drawing::Colour::yellow, Drawing::Colour::saturatedBrown },
        { Drawing::Colour::bordeauxRed, Drawing::Colour::bordeauxRed, Drawing::Colour::oliveGreen },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_SWINGING_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_SWINGING_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "suspended_swinging_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(3, 30), RideRating::make(2, 90), RideRating::make(3, 50) },
        18,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 10), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                32768, 23831, 79437 },
            { RatingsModifierType::bonusTurns,            0,                26749, 34767, 48036 },
            { RatingsModifierType::bonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::bonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                6971, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 8,                2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0xC0000,          2, 2, 2 },
            { RatingsModifierType::requirementNegativeGs, MakeFixed16_2dp(0, 60), 2, 2, 2 },
            { RatingsModifierType::requirementLateralGs,  MakeFixed16_2dp(1, 50), 2, 2, 2 },
            { RatingsModifierType::requirementLength,     0x1720000,        2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                32768, 23831, 79437 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
