/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
constexpr RideTypeDescriptor SwingingInverterShipRTD =
{
    .Category = RIDE_CATEGORY_THRILL,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack1x4B,
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
    .RideModes = EnumsToFlags(RideMode::Swing),
    .DefaultMode = RideMode::Swing,
    .OperatingSettings = { 7, 15 },
    .Naming = { STR_RIDE_NAME_SWINGING_INVERTER_SHIP, STR_RIDE_DESCRIPTION_SWINGING_INVERTER_SHIP },
    .NameConvention = { RideComponentType::Ship, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 15, 176, 7, 11, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 106.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 35,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_BLACK, COLOUR_SATURATED_BROWN, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SWINGING_INVERTER_SHIP_TRACK, SPR_RIDE_DESIGN_PREVIEW_SWINGING_INVERTER_SHIP_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "swinging_inverter_ship",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { MakeRideRating(2, 50), MakeRideRating(2, 70), MakeRideRating(2, 74) },
        16,
        0,
        false,
        {
            { RatingsModifierType::BonusOperationOption, 0, 11, 22, 22 }, 
            { RatingsModifierType::BonusScenery,         0, 11155, 0, 0 },
        },
    },
};
// clang-format on
