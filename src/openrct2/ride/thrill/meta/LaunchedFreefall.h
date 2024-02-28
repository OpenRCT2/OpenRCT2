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
constexpr RideTypeDescriptor LaunchedFreefallRTD =
{
    .AlternateType = RIDE_TYPE_NULL,
    .Category = RIDE_CATEGORY_THRILL,
    .EnabledTrackPieces = {TRACK_TOWER_BASE},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::TowerBase,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionLaunchedFreefall),
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS |
                     RIDE_TYPE_FLAG_HAS_DATA_LOGGING | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN |
                     RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR |
                     RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT,
    .RideModes = EnumsToFlags(RideMode::UpwardLaunch, RideMode::DownwardLaunch),
    .DefaultMode = RideMode::UpwardLaunch,
    .OperatingSettings = { 10, 40, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_LAUNCHED_FREEFALL, STR_RIDE_DESCRIPTION_LAUNCHED_FREEFALL },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_LAUNCHED_FREEFALL),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 32, 3, 2, },
    .MaxMass = 15,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 50, 10 },
    .UpkeepCosts = { 50, 20, 0, 10, 0, 0 },
    .BuildCosts = { 25.00_GBP, 0.00_GBP, 4, },
    .DefaultPrices = { 20, 0 },
    .DefaultMusic = MUSIC_OBJECT_TECHNO,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 65,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_BRIGHT_RED, COLOUR_LIGHT_PURPLE },
        { COLOUR_DARK_GREEN, COLOUR_YELLOW, COLOUR_WHITE },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_LAUNCHED_FREEFALL_TRACK, SPR_RIDE_DESIGN_PREVIEW_LAUNCHED_FREEFALL_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "launched_freefall",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 70), RIDE_RATING(3, 00), RIDE_RATING(3, 50) },
        16,
        -1,
        false,
        {
            { RatingsModifierType::BonusDownwardLaunch,          0, RIDE_RATING(0, 30), RIDE_RATING(0, 65), RIDE_RATING(0, 45) },
            { RatingsModifierType::BonusLaunchedFreefallSpecial, 0, 0, 1355917, 451972 },
            { RatingsModifierType::BonusProximity,               0, 20130, 0, 0 },
            { RatingsModifierType::BonusScenery,                 0, 25098, 0, 0 },
        },
    },
};
// clang-format on
