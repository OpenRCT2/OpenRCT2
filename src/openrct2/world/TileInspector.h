/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Map.h"

enum TILE_INSPECTOR_ELEMENT_TYPE {
    TILE_INSPECTOR_ELEMENT_ANY = 0,
    TILE_INSPECTOR_ELEMENT_SURFACE,
    TILE_INSPECTOR_ELEMENT_PATH,
    TILE_INSPECTOR_ELEMENT_TRACK,
    TILE_INSPECTOR_ELEMENT_SCENERY,
    TILE_INSPECTOR_ELEMENT_ENTRANCE,
    TILE_INSPECTOR_ELEMENT_WALL,
    TILE_INSPECTOR_ELEMENT_SCENERYMULTIPLE,
    TILE_INSPECTOR_ELEMENT_BANNER,
    TILE_INSPECTOR_ELEMENT_CORRUPT,
};

enum TILE_INSPECTOR_INSTRUCTION_TYPE {
    TILE_INSPECTOR_ANY_REMOVE,
    TILE_INSPECTOR_ANY_SWAP,
    TILE_INSPECTOR_ANY_INSERT_CORRUPT,
    TILE_INSPECTOR_ANY_ROTATE,
    TILE_INSPECTOR_ANY_PASTE,
    TILE_INSPECTOR_ANY_SORT,
    TILE_INSPECTOR_ANY_BASE_HEIGHT_OFFSET,
    TILE_INSPECTOR_SURFACE_SHOW_PARK_FENCES,
    TILE_INSPECTOR_SURFACE_TOGGLE_CORNER,
    TILE_INSPECTOR_SURFACE_TOGGLE_DIAGONAL,
    TILE_INSPECTOR_PATH_SET_SLOPE,
    TILE_INSPECTOR_PATH_TOGGLE_EDGE,
    TILE_INSPECTOR_ENTRANCE_MAKE_USABLE,
    TILE_INSPECTOR_WALL_SET_SLOPE,
    TILE_INSPECTOR_TRACK_BASE_HEIGHT_OFFSET,
    TILE_INSPECTOR_TRACK_SET_CHAIN,
    TILE_INSPECTOR_SCENERY_SET_QUARTER_LOCATION,
    TILE_INSPECTOR_SCENERY_SET_QUARTER_COLLISION,
    TILE_INSPECTOR_BANNER_TOGGLE_BLOCKING_EDGE,
    TILE_INSPECTOR_CORRUPT_CLAMP,
};

int32_t tile_inspector_insert_corrupt_at(int32_t x, int32_t y, int16_t elementIndex, int32_t flags);
int32_t tile_inspector_remove_element_at(int32_t x, int32_t y, int16_t elementIndex, int32_t flags);
int32_t tile_inspector_swap_elements_at(int32_t x, int32_t y, int16_t first, int16_t second, int32_t flags);
int32_t tile_inspector_rotate_element_at(int32_t x, int32_t y, int32_t elementIndex, int32_t flags);
int32_t tile_inspector_paste_element_at(int32_t x, int32_t y, rct_tile_element element, int32_t flags);
int32_t tile_inspector_sort_elements_at(int32_t x, int32_t y, int32_t flags);
int32_t tile_inspector_any_base_height_offset(int32_t x, int32_t y, int16_t elementIndex, int8_t heightOffset, int32_t flags);
int32_t tile_inspector_surface_show_park_fences(int32_t x, int32_t y, bool enabled, int32_t flags);
int32_t tile_inspector_surface_toggle_corner(int32_t x, int32_t y, int32_t cornerIndex, int32_t flags);
int32_t tile_inspector_surface_toggle_diagonal(int32_t x, int32_t y, int32_t flags);
int32_t tile_inspector_path_set_sloped(int32_t x, int32_t y, int32_t elementIndex, bool sloped, int32_t flags);
int32_t tile_inspector_path_toggle_edge(int32_t x, int32_t y, int32_t elementIndex, int32_t cornerIndex, int32_t flags);
int32_t tile_inspector_entrance_make_usable(int32_t x, int32_t y, int32_t elementIndex, int32_t flags);
int32_t tile_inspector_wall_set_slope(int32_t x, int32_t y, int32_t elementIndex, int32_t slopeValue, int32_t flags);
int32_t tile_inspector_track_base_height_offset(int32_t x, int32_t y, int32_t elementIndex, int8_t offset, int32_t flags);
int32_t tile_inspector_track_set_chain(int32_t x, int32_t y, int32_t elementIndex, bool entireTrackBlock, bool setChain, int32_t flags);
int32_t tile_inspector_scenery_set_quarter_location(int32_t x, int32_t y, int32_t elementIndex, int32_t quarterIndex, int32_t flags);
int32_t tile_inspector_scenery_set_quarter_collision(int32_t x, int32_t y, int32_t elementIndex, int32_t quarterIndex, int32_t flags);
int32_t tile_inspector_banner_toggle_blocking_edge(int32_t x, int32_t y, int32_t elementIndex, int32_t edgeIndex, int32_t flags);
int32_t tile_inspector_corrupt_clamp(int32_t x, int32_t y, int32_t elementIndex, int32_t flags);
