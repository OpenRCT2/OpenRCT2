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
constexpr RideTypeDescriptor FlyingSaucersRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack4x4,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::flyingSaucers,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourAdditional,
                     RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps, RtdFlag::noTestMode,
                     RtdFlag::noWallsAroundTrack, RtdFlag::isFlatRide, RtdFlag::hasVehicleColours,
                     RtdFlag::hasMusicByDefault, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::singleSession, RtdFlag::interestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::Dodgems),
    .DefaultMode = RideMode::Dodgems,
    .OperatingSettings = { 20, 180 },
    .Naming = { STR_RIDE_NAME_FLYING_SAUCERS, STR_RIDE_DESCRIPTION_FLYING_SAUCERS },
    .NameConvention = { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 9, 48, 2, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 25, 0 },
    .UpkeepCosts = { 90, 1, 0, 5, 0, 0 },
    .BuildCosts = { 35.00_GBP, 2.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 35,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_PURPLE, COLOUR_GREY, COLOUR_BLACK },
        { COLOUR_BLACK, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_YELLOW, COLOUR_SATURATED_RED, COLOUR_BLACK },
        { COLOUR_ICY_BLUE, COLOUR_WHITE, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_FLYING_SAUCERS_TRACK, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "flying_saucers",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { MakeRideRating(2, 40), MakeRideRating(0, 55), MakeRideRating(0, 39) },
        32,
        0,
        false,
        {
            // Special case, passing -2 to represent division by 2
            { RatingsModifierType::BonusOperationOption, 0, 1, -2, 0 }, 
            { RatingsModifierType::BonusNumTrains,       4, MakeRideRating(0, 80), 0, 0 },
            { RatingsModifierType::BonusScenery,         0, 5577, 0, 0 },
        },
    },
};
// clang-format on
