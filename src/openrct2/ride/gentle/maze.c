#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../../paint/supports.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../track_paint.h"
#include "../../paint/map_element/surface.h"
#include "../track.h"

enum {
	SPR_MAZE_BASE_HEDGE = 21938,
	SPR_MAZE_BASE_BRICK = 21951,
	SPR_MAZE_BASE_ICE = 21964,
	SPR_MAZE_BASE_WOOD = 21977,
};

enum {
	SPR_MAZE_OFFSET_WALL_CENTER = 0,
	SPR_MAZE_OFFSET_WALL_INNER_NE_SW,
	SPR_MAZE_OFFSET_WALL_INNER_NW_SE,
	SPR_MAZE_OFFSET_WALL_TOP_LEFT,
	SPR_MAZE_OFFSET_WALL_TOP_RIGHT,
	SPR_MAZE_OFFSET_WALL_BOTTOM_RIGHT,
	SPR_MAZE_OFFSET_WALL_BOTTOM_LEFT,
	SPR_MAZE_OFFSET_COLUMN_CENTER,
	SPR_MAZE_OFFSET_COLUMN_TOP_RIGHT,
	SPR_MAZE_OFFSET_COLUMN_TOP_LEFT,
	SPR_MAZE_OFFSET_COLUMN_BOTTOM_LEFT,
	SPR_MAZE_OFFSET_COLUMN_BOTTOM_RIGHT,
	SPR_MAZE_OFFSET_COLUMN_CORNER,
};

/**
 * rct: 0x004ACF4A
 */
static void maze_paint_setup(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement) {
	uint16 maze_entry = mapElement->properties.track.maze_entry;
	maze_entry = rol16(maze_entry, direction * 4);

	uint32 rotation = get_current_rotation();
	// draw ground
	sint32 image_id = SPR_TERRAIN_DIRT | gTrackColours[SCHEME_MISC];
	sub_98196C(image_id, 0, 0, 32, 32, 0, height, rotation);

	wooden_a_supports_paint_setup((rotation & 1) ? 0 : 1, 0, height, gTrackColours[SCHEME_3], NULL);

	paint_util_set_segment_support_height(SEGMENTS_ALL & ~SEGMENT_C4, 0xFFFF, 0);

	sint32 base_image_id = 0;
	switch (get_ride(rideIndex)->track_colour_supports[0]) {
		case 0: base_image_id = SPR_MAZE_BASE_BRICK; break;
		case 1: base_image_id = SPR_MAZE_BASE_HEDGE; break;
		case 2: base_image_id = SPR_MAZE_BASE_ICE; break;
		case 3: base_image_id = SPR_MAZE_BASE_WOOD; break;
	}

	base_image_id |= gTrackColours[SCHEME_MISC];


	image_id = base_image_id + SPR_MAZE_OFFSET_WALL_CENTER;
	if (maze_entry & (1 << 3))
		sub_98197C(image_id, 2, 2, 10, 10, 9, height, 3, 3, height + 2, rotation);

	if (maze_entry & (1 << 7))
		sub_98197C(image_id, 2, 18, 10, 10, 9, height, 3, 19, height + 2, rotation);

	if (maze_entry & (1 << 11))
		sub_98197C(image_id, 18, 18, 10, 10, 9, height, 19, 19, height + 2, rotation);

	if (maze_entry & (1 << 15))
		sub_98197C(image_id, 18, 2, 10, 10, 9, height, 19, 3, height + 2, rotation);


	image_id = base_image_id + SPR_MAZE_OFFSET_WALL_TOP_LEFT;
	if (maze_entry & (1 << 0))
		sub_98197C(image_id, 2, 0, 10, 1, 9, height, 3, 1, height + 2, rotation);

	if (maze_entry & (1 << 13))
		sub_98197C(image_id, 18, 0, 10, 1, 9, height, 19, 1, height + 2, rotation);


	image_id = base_image_id + SPR_MAZE_OFFSET_WALL_BOTTOM_RIGHT;
	if (maze_entry & (1 << 5))
		sub_98197C(image_id, 2, 30, 10, 1, 9, height, 3, 30, height + 2, rotation);

	if (maze_entry & (1 << 8))
		sub_98197C(image_id, 18, 30, 10, 1, 9, height, 19, 30, height + 2, rotation);


	image_id = base_image_id + SPR_MAZE_OFFSET_WALL_TOP_RIGHT;
	if (maze_entry & (1 << 1))
		sub_98197C(image_id, 0, 2, 1, 10, 9, height, 1, 3, height + 2, rotation);

	if (maze_entry & (1 << 4))
		sub_98197C(image_id, 0, 18, 1, 10, 9, height, 1, 19, height + 2, rotation);


	image_id = base_image_id + SPR_MAZE_OFFSET_WALL_BOTTOM_LEFT;
	if (maze_entry & (1 << 12))
		sub_98197C(image_id, 30, 2, 1, 10, 9, height, 30, 3, height + 2, rotation);

	if (maze_entry & (1 << 9))
		sub_98197C(image_id, 30, 18, 1, 10, 9, height, 30, 19, height + 2, rotation);


	image_id = base_image_id + SPR_MAZE_OFFSET_WALL_INNER_NE_SW;
	if (maze_entry & (1 << 2))
		sub_98197C(image_id, 2, 14, 10, 4, 9, height, 3, 14, height + 2, rotation);

	if (maze_entry & (1 << 10))
		sub_98197C(image_id, 18, 14, 10, 4, 9, height, 19, 14, height + 2, rotation);


	image_id = base_image_id + SPR_MAZE_OFFSET_WALL_INNER_NW_SE;
	if (maze_entry & (1 << 14))
		sub_98197C(image_id, 14, 2, 4, 10, 9, height, 14, 3, height + 2, rotation);

	if (maze_entry & (1 << 6))
		sub_98197C(image_id, 14, 18, 4, 10, 9, height, 14, 19, height + 2, rotation);


	image_id = base_image_id + SPR_MAZE_OFFSET_COLUMN_CORNER;
	if (maze_entry & (1 << 0 | 1 << 1))
		sub_98197C(image_id, 0, 0, 1, 1, 9, height, 1, 1, height + 2, rotation);

	if (maze_entry & (1 << 4 | 1 << 5))
		sub_98197C(image_id, 0, 30, 1, 1, 9, height, 1, 30, height + 2, rotation);

	if (maze_entry & (1 << 8 | 1 << 9))
		sub_98197C(image_id, 30, 30, 1, 1, 9, height, 30, 30, height + 2, rotation);

	if (maze_entry & (1 << 12 | 1 << 13))
		sub_98197C(image_id, 30, 0, 1, 1, 9, height, 30, 1, height + 2, rotation);


	if (maze_entry & (1 << 0 | 1 << 13 | 1 << 14))
		sub_98197C(base_image_id + SPR_MAZE_OFFSET_COLUMN_TOP_LEFT, 14, 0, 2, 1, 9, height, 15, 1, height + 2, rotation);


	if (maze_entry & (1 << 5 | 1 << 6 | 1 << 8))
		sub_98197C(base_image_id + SPR_MAZE_OFFSET_COLUMN_BOTTOM_RIGHT, 14, 30, 2, 1, 9, height, 15, 30, height + 2, rotation);


	if (maze_entry & (1 << 1 | 1 << 2 | 1 << 4))
		sub_98197C(base_image_id + SPR_MAZE_OFFSET_COLUMN_TOP_RIGHT, 0, 14, 1, 2, 9, height, 1, 15, height + 2, rotation);


	if (maze_entry & (1 << 9 | 1 << 10 | 1 << 12))
		sub_98197C(base_image_id + SPR_MAZE_OFFSET_COLUMN_BOTTOM_LEFT, 30, 14, 1, 2, 9, height, 30, 15, height + 2, rotation);


	if (maze_entry & (1 << 2 | 1 << 6 | 1 << 10 | 1 << 14)) {
		sub_98197C(base_image_id + SPR_MAZE_OFFSET_COLUMN_CENTER, 14, 14, 2, 2, 8, height, 15, 15, height + 2, rotation);

		paint_util_set_segment_support_height(SEGMENT_C4, height + 12, 0x20);
	}

	paint_util_set_general_support_height(height + 32, 0x20);
}

/**
 * rct2: 0x008A81E8
 */
TRACK_PAINT_FUNCTION get_track_paint_function_maze(sint32 trackType, sint32 direction) {
	if (trackType != TRACK_ELEM_MAZE) {
		return NULL;
	}

	return maze_paint_setup;
}
