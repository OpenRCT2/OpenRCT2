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
#include "../../../drawing/LightFX.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor DodgemsRTD =
{
    .Category = RideCategory::gentle,
    .StartTrackPiece = TrackElemType::flatTrack4x4,
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
                     RtdFlag::slightlyInterestingToLookAt, RtdFlag::hasRoofOverWholeRide),
    .RideModes = EnumsToFlags(RideMode::dodgems),
    .DefaultMode = RideMode::dodgems,
    .OperatingSettings = { 20, 180 },
    .Naming = { STR_RIDE_NAME_DODGEMS, STR_RIDE_DESCRIPTION_DODGEMS },
    .NameConvention = { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 9, 48, 2, 2, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 40, 20, 0 },
    .UpkeepCosts = { 50, 1, 0, 5, 0, 0 },
    .BuildCosts = { 27.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectDodgems,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 35,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::brightRed, Drawing::Colour::saturatedBrown, Drawing::Colour::saturatedBrown },
        { Drawing::Colour::lightBlue, Drawing::Colour::grey, Drawing::Colour::grey },
        { Drawing::Colour::darkGreen, Drawing::Colour::black, Drawing::Colour::darkGreen },
        { Drawing::Colour::yellow, Drawing::Colour::lightBrown, Drawing::Colour::darkYellow },
        { Drawing::Colour::saturatedGreen, Drawing::Colour::saturatedBrown, Drawing::Colour::saturatedBrown },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_DODGEMS_TRACK, SPR_RIDE_DESIGN_PREVIEW_DODGEMS_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "dodgems",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(1, 30), RideRating::make(0, 50), RideRating::make(0, 35) },
        16,
        7,
        false,
        {
            // Special case, passing -2 to represent division by 2
            { RatingsModifierType::BonusOperationOption, 0, 1, -2, 0 }, 
            { RatingsModifierType::BonusNumTrains,       4, RideRating::make(0, 80), 0, 0 },
            { RatingsModifierType::BonusScenery,         0, 5577, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = Drawing::LightFx::AddLightsMagicVehicle_BoatHire,
};
} // namespace OpenRCT2
// clang-format on
