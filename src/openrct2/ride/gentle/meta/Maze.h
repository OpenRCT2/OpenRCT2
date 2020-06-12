/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor MazeRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_GENTLE),
    SET_FIELD(EnabledTrackPieces, 0),
    SET_FIELD(ExtraTrackPieces, 0),
    SET_FIELD(StartTrackPiece, TRACK_ELEM_MAZE),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_maze),
    SET_FIELD(Flags, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS | RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_NO_TEST_MODE | RIDE_TYPE_FLAG_NO_VEHICLES |
                     RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_IN_RIDE | RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY),
    SET_FIELD(RideModes, (1ULL << RIDE_MODE_MAZE)),
    SET_FIELD(DefaultMode, RIDE_MODE_MAZE),
    SET_FIELD(NameConvention, { RIDE_COMPONENT_TYPE_TRAIN, RIDE_COMPONENT_TYPE_TRACK, RIDE_COMPONENT_TYPE_STATION }),
    SET_FIELD(AvailableBreakdowns, 0),
    SET_FIELD(LiftData, { SoundId::Null, 5, 5 }),
    SET_FIELD(UpkeepCosts, { 50, 1, 0, 0, 0, 0 }),
    SET_FIELD(BuildCosts, { 55, 2, 8, }),
    SET_FIELD(DefaultPrices, { 10, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_STYLE_SUMMER),
    SET_FIELD(PhotoItem, SHOP_ITEM_PHOTO),
};
// clang-format on
