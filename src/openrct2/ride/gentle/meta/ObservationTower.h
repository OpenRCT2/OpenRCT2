/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
constexpr RideTypeDescriptor ObservationTowerRTD =
{
    .AlternateType = RIDE_TYPE_NULL,
    .Category = RIDE_CATEGORY_GENTLE,
    .EnabledTrackPieces = {TRACK_TOWER_BASE},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::TowerBase,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionObservationTower),
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS |
                     RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK |
                     RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT |
                     RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER | RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT,
    .RideModes = EnumsToFlags(RideMode::RotatingLift),
    .DefaultMode = RideMode::RotatingLift,
    .OperatingSettings = { 0, 0, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_OBSERVATION_TOWER, STR_RIDE_DESCRIPTION_OBSERVATION_TOWER },
    .NameConvention = { RideComponentType::Cabin, RideComponentType::Track, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_OBSERVATION_TOWER),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 32, 3, 2, },
    .MaxMass = 15,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 80, 10, 0 },
    .UpkeepCosts = { 50, 20, 0, 10, 0, 0 },
    .BuildCosts = { 18.50_GBP, 0.00_GBP, 4, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = MUSIC_OBJECT_SUMMER,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 45,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BLACK, COLOUR_GREY },
        { COLOUR_LIGHT_BLUE, COLOUR_YELLOW, COLOUR_GREY },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_OBSERVATION_TOWER_TRACK, SPR_RIDE_DESIGN_PREVIEW_OBSERVATION_TOWER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "observation_tower",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(1, 50), RIDE_RATING(0, 00), RIDE_RATING(0, 10) },
        15,
        7,
        false,
        {
            { RatingsModifierType::BonusProximity,         0, 20130, 0, 0 },
            { RatingsModifierType::BonusScenery,           0, 83662, 0, 0 },
            { RatingsModifierType::BonusTowerRide,       0, 45875, 0, 26214 },
            { RatingsModifierType::RequirementUnsheltered, 5, 4, 1, 1 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = LightFxAddLightsMagicVehicle_ObservationTower,
};
// clang-format on
