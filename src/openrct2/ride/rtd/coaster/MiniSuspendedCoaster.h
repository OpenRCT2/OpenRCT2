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
constexpr RideTypeDescriptor MiniSuspendedCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::miniSuspendedCoaster,
        .supportType = MetalSupportType::fork,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::slope, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::diagSlope},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports,
                     RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces, RtdFlag::isSuspended),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 5, 18 },
    .Naming = { STR_RIDE_NAME_MINI_SUSPENDED_COASTER, STR_RIDE_DESCRIPTION_MINI_SUSPENDED_COASTER },
    .NameConvention = { RideComponentType::car, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 10, 24, 24, 8, },
    .MaxMass = 3,
    .LiftData = { Audio::SoundId::liftFrictionWheels, 4, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 10, 3, 10 },
    .BuildCosts = { 30.00_GBP, 2.50_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectWildWest,
    .PhotoItem = ShopItem::photo2,
    .BonusValue = 50,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::black, Drawing::Colour::black, Drawing::Colour::saturatedBrown },
        { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::black },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MINI_SUSPENDED_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_MINI_SUSPENDED_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "mini_suspended_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(2, 80), RideRating::make(2, 50), RideRating::make(2, 70) },
        15,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 45), RideRating::make(0, 15), 0 },
            { RatingsModifierType::bonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                24576, 35746, 49648 },
            { RatingsModifierType::bonusTurns,            0,                34179, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                58254, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                19275, 32768, 35108 },
            { RatingsModifierType::bonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                13943, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 6,                2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0x80000,          2, 2, 2 },
            { RatingsModifierType::requirementLateralGs,  MakeFixed16_2dp(1, 30), 2, 2, 2 },
            { RatingsModifierType::requirementLength,     0xC80000,         2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                24576, 35746, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
