#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"
#include "map.h"

typedef enum {
	TILE_INSPECTOR_ELEMENT_ANY = 0,
	TILE_INSPECTOR_ELEMENT_SURFACE,
	TILE_INSPECTOR_ELEMENT_PATH,
	TILE_INSPECTOR_ELEMENT_TRACK,
	TILE_INSPECTOR_ELEMENT_SCENERY,
	TILE_INSPECTOR_ELEMENT_ENTRANCE,
	TILE_INSPECTOR_ELEMENT_FENCE,
	TILE_INSPECTOR_ELEMENT_SCENERYMULTIPLE,
	TILE_INSPECTOR_ELEMENT_BANNER,
	TILE_INSPECTOR_ELEMENT_CORRUPT,
} tile_inspector_element_type;

typedef enum {
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
	TILE_INSPECTOR_PATH_TOGGLE_EDGE,
	TILE_INSPECTOR_FENCE_SET_SLOPE,
} tile_inspector_instruction;

sint32 tile_inspector_insert_corrupt_at(sint32 x, sint32 y, sint16 element_index, sint32 flags);
sint32 tile_inspector_remove_element_at(sint32 x, sint32 y, sint16 element_index, sint32 flags);
sint32 tile_inspector_swap_elements_at(sint32 x, sint32 y, sint16 first, sint16 second, sint32 flags);
sint32 tile_inspector_rotate_element_at(sint32 x, sint32 y, sint32 element_index, sint32 flags);
sint32 tile_inspector_paste_element_at(sint32 x, sint32 y, rct_map_element element, sint32 flags);
sint32 tile_inspector_sort_elements_at(sint32 x, sint32 y, sint32 flags);
sint32 tile_inspector_change_base_height_at(sint32 x, sint32 y, sint16 element_index, sint8 height_offset, sint32 flags);
sint32 tile_inspector_surface_show_park_fences(sint32 x, sint32 y, bool enabled, sint32 flags);
sint32 tile_inspector_surface_toggle_corner(sint32 x, sint32 y, sint32 corner_index, sint32 flags);
sint32 tile_inspector_surface_toggle_diagonal(sint32 x, sint32 y, sint32 flags);
sint32 tile_inspector_path_toggle_edge(sint32 x, sint32 y, sint32 element_index, sint32 corner_index, sint32 flags);
sint32 tile_inspector_fence_set_slope(sint32 x, sint32 y, sint32 element_index, sint32 slope_value, sint32 flags);
