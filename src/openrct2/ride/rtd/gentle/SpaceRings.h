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
constexpr RideTypeDescriptor SpaceRingsRTD =
{
    .Category = RideCategory::gentle,
    .StartTrackPiece = TrackElemType::flatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::spaceRings,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps, RtdFlag::noTestMode,
                     RtdFlag::noWallsAroundTrack, RtdFlag::isFlatRide, RtdFlag::hasVehicleColours,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::slightlyInterestingToLookAt, RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::spaceRings),
    .DefaultMode = RideMode::spaceRings,
    .Naming = { STR_RIDE_NAME_SPACE_RINGS, STR_RIDE_DESCRIPTION_SPACE_RINGS },
    .NameConvention = { RideComponentType::Ring, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 16, 48, 3, 2, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 12, 4, 4 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 36.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 5, 0 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 30,
    .ColourPresets = kDefaultFlatRideColourPreset,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "space_rings",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(1, 50), RideRating::make(2, 10), RideRating::make(6, 50) },
        7,
        0,
        false,
        {
            { RatingsModifierType::BonusScenery,          0,                25098, 0, 0 },
        },
    },
    .specialType = RtdSpecialType::spaceRings,
};
} // namespace OpenRCT2
// clang-format on
