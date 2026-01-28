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
constexpr RideTypeDescriptor SwingingShipRTD =
{
    .Category = RideCategory::thrill,
    .StartTrackPiece = TrackElemType::flatTrack1x5,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::swingingShip,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports,
                     RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps,
                     RtdFlag::hasLoadOptions, RtdFlag::vehicleIsIntegral, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::hasVehicleColours, RtdFlag::allowMusic,
                     RtdFlag::hasEntranceAndExit, RtdFlag::singleSession, RtdFlag::interestingToLookAt,
                     RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::swing),
    .DefaultMode = RideMode::swing,
    .OperatingSettings = { 7, 25 },
    .Naming = { STR_RIDE_NAME_SWINGING_SHIP, STR_RIDE_DESCRIPTION_SWINGING_SHIP },
    .NameConvention = { RideComponentType::Ship, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 12, 112, 7, 11, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 77.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectPirate,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 35,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::darkBrown, Drawing::Colour::darkBrown, Drawing::Colour::black },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SWINGING_SHIP_TRACK, SPR_RIDE_DESIGN_PREVIEW_SWINGING_SHIP_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "swinging_ship",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(1, 50), RideRating::make(1, 90), RideRating::make(1, 41) },
        10,
        0,
        false,
        {
            { RatingsModifierType::BonusOperationOption, 0, 5, 5, 10 }, 
            { RatingsModifierType::BonusScenery,         0, 16732, 0, 0 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
