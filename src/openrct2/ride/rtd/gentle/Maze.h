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
#include "../../RideAudio.h"
#include "../../RideData.h"
#include "../../RideRatings.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor MazeRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::Maze,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionMaze,
        .EnabledTrackPieces = {},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourSupports, RtdFlag::hasSinglePieceStation, RtdFlag::noTestMode, RtdFlag::noVehicles,
                     RtdFlag::noWallsAroundTrack, RtdFlag::describeAsInside, RtdFlag::hasTrack, RtdFlag::hasEntranceAndExit,
                     RtdFlag::guestsCanUseUmbrella, RtdFlag::isMaze),
    .RideModes = EnumsToFlags(RideMode::Maze),
    .DefaultMode = RideMode::Maze,
    .OperatingSettings = { 1, 64 },
    .Naming = { STR_RIDE_NAME_MAZE, STR_RIDE_DESCRIPTION_MAZE },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_MAZE",
    .AvailableBreakdowns = 0,
    .Heights = { 6, 24, 0, 1, },
    .MaxMass = 18,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 0, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 27.50_GBP, 1.00_GBP, 8, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = MUSIC_OBJECT_SUMMER,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 40,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_GREY, COLOUR_GREY, COLOUR_GREY },
    ),
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "maze",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RIDE_RATING(1, 30), RIDE_RATING(0, 50), RIDE_RATING(0, 00) },
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
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Maze,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceMaze,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Maze,
};
// clang-format on
