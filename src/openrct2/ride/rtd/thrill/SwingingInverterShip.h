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
constexpr RideTypeDescriptor SwingingInverterShipRTD =
{
    .Category = RideCategory::thrill,
    .StartTrackPiece = TrackElemType::flatTrack1x4B,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::swingingInverterShip,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasSinglePieceStation,
                     RtdFlag::cannotHaveGaps, RtdFlag::hasLoadOptions, RtdFlag::vehicleIsIntegral,
                     RtdFlag::noWallsAroundTrack, RtdFlag::isFlatRide, RtdFlag::hasVehicleColours,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::interestingToLookAt, RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::swing),
    .DefaultMode = RideMode::swing,
    .OperatingSettings = { 7, 15 },
    .Naming = { STR_RIDE_NAME_SWINGING_INVERTER_SHIP, STR_RIDE_DESCRIPTION_SWINGING_INVERTER_SHIP },
    .NameConvention = { RideComponentType::Ship, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 15, 176, 7, 11, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 106.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 35,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::bordeauxRed, Drawing::Colour::yellow, Drawing::Colour::black },
        { Drawing::Colour::black, Drawing::Colour::saturatedBrown, Drawing::Colour::black },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SWINGING_INVERTER_SHIP_TRACK, SPR_RIDE_DESIGN_PREVIEW_SWINGING_INVERTER_SHIP_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "swinging_inverter_ship",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(2, 50), RideRating::make(2, 70), RideRating::make(2, 74) },
        16,
        0,
        false,
        {
            { RatingsModifierType::BonusOperationOption, 0, 11, 22, 22 }, 
            { RatingsModifierType::BonusScenery,         0, 11155, 0, 0 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
