/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
constexpr const RideTypeDescriptor LiftRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_TRANSPORT),
    SET_FIELD(EnabledTrackPieces, {TRACK_TOWER_BASE}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::TowerBase),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_lift),
    SET_FIELD(Flags, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS | RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION |
                     RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS |
                     RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_ALLOW_EXTRA_TOWER_BASES | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR |
                     RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_TRANSPORT_RIDE | RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::Shuttle)),
    SET_FIELD(DefaultMode, RideMode::Shuttle),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_LIFT, STR_RIDE_DESCRIPTION_LIFT }),
    SET_FIELD(NameConvention, { RideComponentType::Cabin, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_LIFT)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_DOORS_STUCK_CLOSED) | (1 << BREAKDOWN_DOORS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 255, 32, 3, 2, }),
    SET_FIELD(MaxMass, 15),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_lift),
    SET_FIELD(RatingsMultipliers, { 80, 10, 0 }),
    SET_FIELD(UpkeepCosts, { 50, 20, 0, 10, 0, 0 }),
    SET_FIELD(BuildCosts, { 19.50_GBP, 0.00_GBP, 4, }),
    SET_FIELD(DefaultPrices, { 10, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_SUMMER),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 45),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_GREY },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_GREY },
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_GREY },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_LIFT_TRACK, 0 }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "lift"),
};
// clang-format on
