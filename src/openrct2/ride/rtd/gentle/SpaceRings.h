/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
constexpr RideTypeDescriptor SpaceRingsRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionSpaceRings,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps, RtdFlag::noTestMode,
                     RtdFlag::noWallsAroundTrack, RtdFlag::isFlatRide, RtdFlag::hasVehicleColours,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::slightlyInterestingToLookAt, RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::SpaceRings),
    .DefaultMode = RideMode::SpaceRings,
    .Naming = { STR_RIDE_NAME_SPACE_RINGS, STR_RIDE_DESCRIPTION_SPACE_RINGS },
    .NameConvention = { RideComponentType::Ring, RideComponentType::Structure, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_SPACE_RINGS",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 16, 48, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 12, 4, 4 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 36.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 5, 0 },
    .DefaultMusic = MUSIC_OBJECT_GENTLE,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 30,
    .ColourPresets = DEFAULT_FLAT_RIDE_COLOUR_PRESET,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "space_rings",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RIDE_RATING(1, 50), RIDE_RATING(2, 10), RIDE_RATING(6, 50) },
        7,
        0,
        false,
        {
            { RatingsModifierType::BonusScenery,          0,                25098, 0, 0 },
        },
    },
};
// clang-format on
