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
constexpr RideTypeDescriptor kGoKartsRTD =
{
    .AlternateType = kRideTypeNull,
    .Category = RIDE_CATEGORY_THRILL,
    .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_CURVE_VERY_SMALL},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::kEndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionGoKarts),
    .Flags = RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS | RIDE_TYPE_FLAG_NO_TEST_MODE | RIDE_TYPE_FLAG_HAS_ONE_STATION |
                     RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK |
                     RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT,
    .RideModes = EnumsToFlags(RideMode::Race, RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::Race,
    .OperatingSettings = { 1, 10, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_GO_KARTS, STR_RIDE_DESCRIPTION_GO_KARTS },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .kEnumName = nameof(RIDE_TYPE_GO_KARTS),
    .AvailableBreakdowns = (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 7, 24, 2, 1, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 120, 20, 0 },
    .UpkeepCosts = { 50, 20, 0, 8, 0, 0 },
    .BuildCosts = { 31.00_GBP, 2.00_GBP, 20, },
    .DefaultPrices = { 20, 0 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_DARK_BROWN },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BORDEAUX_RED },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_DARK_GREEN },
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_TRACK, SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "go_karts",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(1, 42), RIDE_RATING(1, 73), RIDE_RATING(0, 40) },
        16,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,            700, 32768, 0, 0 },
            { RatingsModifierType::BonusGoKartRace,        4,   RIDE_RATING(1, 40), RIDE_RATING(0, 50), 0 },
            { RatingsModifierType::BonusTurns,             0,   4458, 3476, 5718 },
            { RatingsModifierType::BonusDrops,             0,   8738, 5461, 6553 },
            { RatingsModifierType::BonusSheltered,         0,   2570, 8738, 2340 },
            { RatingsModifierType::BonusProximity,         0,   11183, 0, 0 },
            { RatingsModifierType::BonusScenery,           0,   16732, 0, 0 },
            { RatingsModifierType::RequirementUnsheltered, 6,   2, 1, 1 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = LightFxAddLightsMagicVehicle_BoatHire,
};
// clang-format on
