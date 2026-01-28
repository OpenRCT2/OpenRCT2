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
#include "../../RideAudio.h"
#include "../../RideData.h"
#include "../../RideRatings.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor MazeRTD =
{
    .Category = RideCategory::gentle,
    .StartTrackPiece = TrackElemType::maze,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::maze,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourSupports, RtdFlag::hasSinglePieceStation, RtdFlag::noTestMode, RtdFlag::noVehicles,
                     RtdFlag::noWallsAroundTrack, RtdFlag::describeAsInside, RtdFlag::hasTrack, RtdFlag::hasEntranceAndExit,
                     RtdFlag::guestsCanUseUmbrella),
    .RideModes = EnumsToFlags(RideMode::maze),
    .DefaultMode = RideMode::maze,
    .OperatingSettings = { 1, 64 },
    .Naming = { STR_RIDE_NAME_MAZE, STR_RIDE_DESCRIPTION_MAZE },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = 0,
    .Heights = { 6, 24, 0, 1, },
    .MaxMass = 18,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 50, 0, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 27.50_GBP, 1.00_GBP, 8, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 40,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::grey, Drawing::Colour::grey, Drawing::Colour::grey },
    ),
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "maze",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(1, 30), RideRating::make(0, 50), RideRating::make(0, 00) },
        8,
        0,
        false,
        {
            { RatingsModifierType::BonusMazeSize, 100, 1, 2, 0 },
            { RatingsModifierType::BonusScenery,  0,   22310, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Maze,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceMaze,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Maze,
    .specialType = RtdSpecialType::maze,
};
} // namespace OpenRCT2
// clang-format on
