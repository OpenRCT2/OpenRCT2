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
constexpr RideTypeDescriptor kMiniGolfRTD =
{
    .AlternateType = RIDE_TYPE_NULL,
    .Category = RIDE_CATEGORY_GENTLE,
    .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_CURVE_VERY_SMALL, TRACK_MINI_GOLF_HOLE},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionMiniGolf),
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_NO_TEST_MODE | RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_HAS_ONE_STATION |
                     RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT |
                     RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 0, 0, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_MINI_GOLF, STR_RIDE_DESCRIPTION_MINI_GOLF },
    .NameConvention = { RideComponentType::Player, RideComponentType::Course, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_MINI_GOLF),
    .AvailableBreakdowns = 0,
    .Heights = { 7, 32, 2, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 30, 60, 0, 0, 0, 0 },
    .BuildCosts = { 25.00_GBP, 3.50_GBP, 20, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = MUSIC_OBJECT_SUMMER,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 23,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_SATURATED_GREEN, COLOUR_DARK_BROWN, COLOUR_BLACK },
        { COLOUR_MOSS_GREEN, COLOUR_BORDEAUX_RED, COLOUR_DARK_BROWN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MINI_GOLF_TRACK, SPR_RIDE_DESIGN_PREVIEW_MINI_GOLF_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "mini_golf",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(1, 50), RIDE_RATING(0, 90), RIDE_RATING(0, 00) },
        0,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,      6000, 873, 0, 0 },
            { RatingsModifierType::BonusTurns,       0,    14860, 0, 0 },
            { RatingsModifierType::BonusSheltered,   0,    5140, 6553, 4681 },
            { RatingsModifierType::BonusProximity,   0,    15657, 0, 0 },
            { RatingsModifierType::BonusScenery,     0,    27887, 0, 0 },
            { RatingsModifierType::BonusHoles,       0,    5, 0, 0 },
            { RatingsModifierType::RequirementHoles, 1,    8, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = nullptr,
    .UpdateMeasurementsSpecialElements = RideUpdateMeasurementsSpecialElements_MiniGolf,
};
// clang-format on
