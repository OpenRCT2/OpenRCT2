/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../drawing/LightFX.h"
#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor DodgemsRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack4x4,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::dodgems,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasSinglePieceStation,
                     RtdFlag::cannotHaveGaps, RtdFlag::noTestMode, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::hasVehicleColours, RtdFlag::hasMusicByDefault,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::slightlyInterestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::Dodgems),
    .DefaultMode = RideMode::Dodgems,
    .OperatingSettings = { 20, 180 },
    .Naming = { STR_RIDE_NAME_DODGEMS, STR_RIDE_DESCRIPTION_DODGEMS },
    .NameConvention = { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 9, 48, 2, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 40, 20, 0 },
    .UpkeepCosts = { 50, 1, 0, 5, 0, 0 },
    .BuildCosts = { 27.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectDodgems,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 35,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_GREY, COLOUR_GREY },
        { COLOUR_DARK_GREEN, COLOUR_BLACK, COLOUR_DARK_GREEN },
        { COLOUR_YELLOW, COLOUR_LIGHT_BROWN, COLOUR_DARK_YELLOW },
        { COLOUR_SATURATED_GREEN, COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_DODGEMS_TRACK, SPR_RIDE_DESIGN_PREVIEW_DODGEMS_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "dodgems",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { MakeRideRating(1, 30), MakeRideRating(0, 50), MakeRideRating(0, 35) },
        16,
        7,
        false,
        {
            // Special case, passing -2 to represent division by 2
            { RatingsModifierType::BonusOperationOption, 0, 1, -2, 0 }, 
            { RatingsModifierType::BonusNumTrains,       4, MakeRideRating(0, 80), 0, 0 },
            { RatingsModifierType::BonusScenery,         0, 5577, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_BoatHire,
};
// clang-format on
