/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
GameActionResultPtr tile_inspector_insert_corrupt_at(const CoordsXY& loc, int16_t elementIndex, bool isExecuting);
GameActionResultPtr tile_inspector_remove_element_at(const CoordsXY& loc, int16_t elementIndex, bool isExecuting);
GameActionResultPtr tile_inspector_swap_elements_at(const CoordsXY& loc, int16_t first, int16_t second, bool isExecuting);
GameActionResultPtr tile_inspector_rotate_element_at(const CoordsXY& loc, int32_t elementIndex, bool isExecuting);
GameActionResultPtr tile_inspector_paste_element_at(const CoordsXY& loc, TileElement element, bool isExecuting);
GameActionResultPtr tile_inspector_sort_elements_at(const CoordsXY& loc, bool isExecuting);
GameActionResultPtr tile_inspector_any_base_height_offset(
    const CoordsXY& loc, int16_t elementIndex, int8_t heightOffset, bool isExecuting);
GameActionResultPtr tile_inspector_surface_show_park_fences(const CoordsXY& loc, bool enabled, bool isExecuting);
GameActionResultPtr tile_inspector_surface_toggle_corner(const CoordsXY& loc, int32_t cornerIndex, bool isExecuting);
GameActionResultPtr tile_inspector_surface_toggle_diagonal(const CoordsXY& loc, bool isExecuting);
GameActionResultPtr tile_inspector_path_set_sloped(const CoordsXY& loc, int32_t elementIndex, bool sloped, bool isExecuting);
GameActionResultPtr tile_inspector_path_set_broken(const CoordsXY& loc, int32_t elementIndex, bool broken, bool isExecuting);
GameActionResultPtr tile_inspector_path_toggle_edge(
    const CoordsXY& loc, int32_t elementIndex, int32_t cornerIndex, bool isExecuting);
GameActionResultPtr tile_inspector_entrance_make_usable(const CoordsXY& loc, int32_t elementIndex, bool isExecuting);
GameActionResultPtr tile_inspector_wall_set_slope(
    const CoordsXY& loc, int32_t elementIndex, int32_t slopeValue, bool isExecuting);
GameActionResultPtr tile_inspector_wall_animation_frame_offset(
    const CoordsXY& loc, int16_t elementIndex, int8_t animationFrameOffset, bool isExecuting);
GameActionResultPtr tile_inspector_track_base_height_offset(
    const CoordsXY& loc, int32_t elementIndex, int8_t offset, bool isExecuting);
GameActionResultPtr tile_inspector_track_set_block_brake(
    const CoordsXY& loc, int32_t elementIndex, bool blockBrake, bool isExecuting);
GameActionResultPtr tile_inspector_track_set_indestructible(
    const CoordsXY& loc, int32_t elementIndex, bool isIndestructible, bool isExecuting);
GameActionResultPtr tile_inspector_track_set_chain(
    const CoordsXY& loc, int32_t elementIndex, bool entireTrackBlock, bool setChain, bool isExecuting);
GameActionResultPtr tile_inspector_scenery_set_quarter_location(
    const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting);
GameActionResultPtr tile_inspector_scenery_set_quarter_collision(
    const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting);
GameActionResultPtr tile_inspector_banner_toggle_blocking_edge(
    const CoordsXY& loc, int32_t elementIndex, int32_t edgeIndex, bool isExecuting);
GameActionResultPtr tile_inspector_corrupt_clamp(const CoordsXY& loc, int32_t elementIndex, bool isExecuting);
