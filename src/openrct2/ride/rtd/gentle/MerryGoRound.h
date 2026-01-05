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
constexpr RideTypeDescriptor MerryGoRoundRTD =
{
    .Category = RideCategory::gentle,
    .StartTrackPiece = TrackElemType::flatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::merryGoRound,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps, 
                     RtdFlag::hasLoadOptions, RtdFlag::vehicleIsIntegral, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::hasVehicleColours, RtdFlag::hasMusicByDefault,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::interestingToLookAt, RtdFlag::listVehiclesSeparately,
                     RtdFlag::requireExplicitListingInMusicObjects),
    .RideModes = EnumsToFlags(RideMode::rotation),
    .DefaultMode = RideMode::rotation,
    .OperatingSettings = { 4, 25 },
    .Naming = { STR_RIDE_NAME_MERRY_GO_ROUND, STR_RIDE_DESCRIPTION_MERRY_GO_ROUND },
    .NameConvention = { RideComponentType::Car, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_CONTROL_FAILURE),
    .Heights = { 12, 64, 3, 2, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 50, 10, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 57.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectFairground,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 45,
    .ColourPresets = kDefaultFlatRideColourPreset,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "merry_go_round",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(0, 60), RideRating::make(0, 15), RideRating::make(0, 30) },
        16,
        7,
        false,
        {
            { RatingsModifierType::BonusRotations,        0,                5, 5, 5 },
            { RatingsModifierType::BonusScenery,          0,                19521, 0, 0 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
