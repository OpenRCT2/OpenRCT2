/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Map.h"

enum TILE_INSPECTOR_ELEMENT_TYPE
{
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

class GameActionResult;
using GameActionResultPtr = std::unique_ptr<GameActionResult>;
GameActionResultPtr tile_inspector_insert_corrupt_at(int32_t x, int32_t y, int16_t elementIndex, bool isExecuting);
GameActionResultPtr tile_inspector_remove_element_at(int32_t x, int32_t y, int16_t elementIndex, bool isExecuting);
GameActionResultPtr tile_inspector_swap_elements_at(int32_t x, int32_t y, int16_t first, int16_t second, bool isExecuting);
GameActionResultPtr tile_inspector_rotate_element_at(int32_t x, int32_t y, int32_t elementIndex, bool isExecuting);
GameActionResultPtr tile_inspector_paste_element_at(int32_t x, int32_t y, TileElement element, bool isExecuting);
GameActionResultPtr tile_inspector_sort_elements_at(int32_t x, int32_t y, bool isExecuting);
GameActionResultPtr tile_inspector_any_base_height_offset(
    int32_t x, int32_t y, int16_t elementIndex, int8_t heightOffset, bool isExecuting);
GameActionResultPtr tile_inspector_surface_show_park_fences(int32_t x, int32_t y, bool enabled, bool isExecuting);
GameActionResultPtr tile_inspector_surface_toggle_corner(int32_t x, int32_t y, int32_t cornerIndex, bool isExecuting);
GameActionResultPtr tile_inspector_surface_toggle_diagonal(int32_t x, int32_t y, bool isExecuting);
GameActionResultPtr tile_inspector_path_set_sloped(int32_t x, int32_t y, int32_t elementIndex, bool sloped, bool isExecuting);
GameActionResultPtr tile_inspector_path_set_broken(int32_t x, int32_t y, int32_t elementIndex, bool broken, bool isExecuting);
GameActionResultPtr tile_inspector_path_toggle_edge(
    int32_t x, int32_t y, int32_t elementIndex, int32_t cornerIndex, bool isExecuting);
GameActionResultPtr tile_inspector_entrance_make_usable(int32_t x, int32_t y, int32_t elementIndex, bool isExecuting);
GameActionResultPtr tile_inspector_wall_set_slope(
    int32_t x, int32_t y, int32_t elementIndex, int32_t slopeValue, bool isExecuting);
GameActionResultPtr tile_inspector_track_base_height_offset(
    int32_t x, int32_t y, int32_t elementIndex, int8_t offset, bool isExecuting);
GameActionResultPtr tile_inspector_track_set_block_brake(
    int32_t x, int32_t y, int32_t elementIndex, bool blockBrake, bool isExecuting);
GameActionResultPtr tile_inspector_track_set_indestructible(
    int32_t x, int32_t y, int32_t elementIndex, bool isIndestructible, bool isExecuting);
GameActionResultPtr tile_inspector_track_set_chain(
    int32_t x, int32_t y, int32_t elementIndex, bool entireTrackBlock, bool setChain, bool isExecuting);
GameActionResultPtr tile_inspector_scenery_set_quarter_location(
    int32_t x, int32_t y, int32_t elementIndex, int32_t quarterIndex, bool isExecuting);
GameActionResultPtr tile_inspector_scenery_set_quarter_collision(
    int32_t x, int32_t y, int32_t elementIndex, int32_t quarterIndex, bool isExecuting);
GameActionResultPtr tile_inspector_banner_toggle_blocking_edge(
    int32_t x, int32_t y, int32_t elementIndex, int32_t edgeIndex, bool isExecuting);
GameActionResultPtr tile_inspector_corrupt_clamp(int32_t x, int32_t y, int32_t elementIndex, bool isExecuting);
