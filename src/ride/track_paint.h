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

#ifndef _TRACK_PAINT_H
#define _TRACK_PAINT_H

#include "ride_data.h"
#include "../common.h"
#include "../world/map.h"
#include "../paint/map_element/map_element.h"

extern const uint8 track_map_2x2[][4];
extern const uint8 edges_2x2[];

extern const uint8 track_map_3x3[][9];
extern const uint8 edges_3x3[];

extern const uint8 track_map_4x4[][16];
extern const uint8 edges_4x4[];

enum {
	SPR_FLOOR_METAL = 14567,
	SPR_FENCE_METAL_NE = 14568,
	SPR_FENCE_METAL_SE = 14569,
	SPR_FENCE_METAL_SW = 14570,
	SPR_FENCE_METAL_NW = 14571,

    SPR_FLOOR_CORK_SE_SW = 22134,
    SPR_FLOOR_CORK_SW = 22135,
    SPR_FLOOR_CORK_SE = 22136,
    SPR_FLOOR_CORK = 22137,
    SPR_FENCE_ROPE_NE = 22138,
    SPR_FENCE_ROPE_SE = 22139,
    SPR_FENCE_ROPE_SW = 22140,
    SPR_FENCE_ROPE_NW = 22141,

    SPR_STATION_PIER_EDGE_SE = 22404,
    SPR_STATION_PIER_EDGE_SW = 22405,
    SPR_STATION_PIER_EDGE_NW = 22406,
    SPR_STATION_PIER_EDGE_NE = 22407,
    SPR_STATION_PIER_EDGE_NW_FENCED = 22408,
    SPR_STATION_PIER_EDGE_NE_FENCED = 22409,
    SPR_STATION_PIER_FENCE_SE = 22410,
    SPR_STATION_PIER_FENCE_SW = 22411,
};

extern const uint32 floorSpritesCork[];

extern const uint32 fenceSpritesRope[];

bool track_paint_util_has_fence(enum edge edge, rct_xy16 position, rct_map_element * mapElement, rct_ride * ride, uint8 rotation);
void track_paint_util_paint_floor(uint8 edges, uint32 colourFlags, uint16 height, const uint32 floorSprites[4], uint8 rotation);
void track_paint_util_paint_fences(uint8 edges, rct_xy16 position, rct_map_element * mapElement, rct_ride * ride, uint32 colourFlags, uint16 height, const uint32 fenceSprites[4], uint8 rotation);
bool track_paint_util_draw_station_covers(enum edge edge, bool hasFence, const rct_ride_entrance_definition * entranceStyle, uint8 direction, uint16 height);
bool track_paint_util_should_paint_supports(rct_xy16 position);

typedef void (*TRACK_PAINT_FUNCTION)(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement);
typedef TRACK_PAINT_FUNCTION (*TRACK_PAINT_FUNCTION_GETTER)(int trackType, int direction);

TRACK_PAINT_FUNCTION get_track_paint_function_chairlift(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_maze(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_spiral_slide(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_dodgems(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_ferris_wheel(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_3d_cinema(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_topspin(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_shop(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_merry_go_round(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_facility(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_haunted_house(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_circus_show(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_flying_saucers(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_crooked_house(int trackType, int direction);
TRACK_PAINT_FUNCTION get_track_paint_function_submarine_ride(int trackType, int direction);

#endif
