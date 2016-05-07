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

#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../track_paint.h"

/* rct2: 0x007667AE */
static rct_xy16 loc_7667AE[] = {
	{ .x = 0,  .y = -1 },
	{ .x = 1,  .y = 0 },
	{ .x = 0,  .y = 1},
	{ .x = -1, .y = 0 },
};

/* rct2: 0x007667AC */
static rct_xy16 loc_7667AC[] = {
	{ .x = -1, .y = 0 },
	{ .x = 0,  .y = -1 },
	{ .x = 1,  .y = 0 },
	{ .x = 0,  .y = 1 },
};

enum {
	DODGEMS_FLOOR = 21925,
	DODGEMS_ROOF_FRAME = 21926, // 4 directions
	DODGEMS_ROOF_GLASS = 21930, // 4 directions
	DODGEMS_FENCE_TOP_RIGHT = 21934,
	DODGEMS_FENCE_BOTTOM_RIGHT = 21935,
	DODGEMS_FENCE_BOTTOM_LEFT = 21936,
	DODGEMS_FENCE_TOP_LEFT = 21937
};

static void dodgems_paint_sub_floor(uint8 direction, int height) {
	uint32 image_id = RCT2_GLOBAL(0x00F441A0, uint32);
	wooden_a_supports_paint_setup(direction & 1, 0, height, image_id, NULL);

	image_id = DODGEMS_FLOOR | RCT2_GLOBAL(0x00F4419C, uint32);
	sub_98197C(image_id, 0, 0, 30, 30, 1, height, 1, 1, height, get_current_rotation());
}

static void dodgems_paint_sub_fence_top_left(uint8 rideIndex, int height, rct_map_element *mapElement) {
	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);
	uint16 entranceLoc =
		((x / 32) + loc_7667AE[get_current_rotation()].x) |
		(((y / 32) + loc_7667AE[get_current_rotation()].y) << 8);

	uint8 entranceId = (mapElement->properties.track.sequence & 0x70) >> 4;
	rct_ride *ride = get_ride(rideIndex);

	if (ride->entrances[entranceId] == entranceLoc || ride->exits[entranceId] == entranceLoc) {
		return;
	}

	uint32 image_id = DODGEMS_FENCE_TOP_LEFT | RCT2_GLOBAL(0x00F4419C, uint32);
	sub_98199C(image_id, 0, 0, 32, 1, 7, height, 0, 2, height + 2, get_current_rotation());
}

static void dodgems_paint_sub_fence_top_right(uint8 rideIndex, int height, rct_map_element *mapElement) {
	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);
	uint16 entranceLoc =
		((x / 32) + loc_7667AC[get_current_rotation()].x) |
		(((y / 32) + loc_7667AC[get_current_rotation()].y) << 8);

	uint8 entranceId = (mapElement->properties.track.sequence & 0x70) >> 4;
	rct_ride *ride = get_ride(rideIndex);

	if (ride->entrances[entranceId] == entranceLoc || ride->exits[entranceId] == entranceLoc) {
		return;
	}

	uint32 image_id = DODGEMS_FENCE_TOP_RIGHT | RCT2_GLOBAL(0x00F4419C, uint32);
	sub_98199C(image_id, 0, 0, 1, 32, 7, height, 2, 0, height + 2, get_current_rotation());
}

static void dodgems_paint_sub_fence_bottom_right(uint8 rideIndex, int height, rct_map_element *mapElement) {
	uint32 image_id = DODGEMS_FENCE_BOTTOM_RIGHT | RCT2_GLOBAL(0x00F4419C, uint32);
	sub_98197C(image_id, 0, 0, 32, 1, 7, height, 0, 30, height + 2, get_current_rotation());
}

static void dodgems_paint_sub_fence_bottom_left(uint8 rideIndex, int height, rct_map_element *mapElement) {
	uint32 image_id = DODGEMS_FENCE_BOTTOM_LEFT | RCT2_GLOBAL(0x00F4419C, uint32);
	sub_98197C(image_id, 0, 0, 1, 13, 7, height, 30, 0, height + 2, get_current_rotation());
}

static void dodgems_paint_sub_roof(int height, int offset) {
	uint32 image_id = (DODGEMS_ROOF_FRAME + offset) | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98196C(image_id, 0, 0, 32, 32, 2, height, get_current_rotation());

	image_id = (DODGEMS_ROOF_GLASS + offset) | (0xF << 19) | (0x1 << 24) | 0x40000000;
	paint_attach_to_previous_ps(image_id, 0, 0);
}

static void dodgems_paint_setup_rot_0(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	dodgems_paint_sub_floor(direction, height);

	switch (trackSequence) {
		case 0:
			dodgems_paint_sub_fence_top_left(rideIndex, height, mapElement);
			dodgems_paint_sub_fence_top_right(rideIndex, height, mapElement);
			break;

		case 1:
		case 2:
			dodgems_paint_sub_fence_top_right(rideIndex, height, mapElement);
			break;

		case 3:
			dodgems_paint_sub_fence_top_right(rideIndex, height, mapElement);
			dodgems_paint_sub_fence_bottom_right(rideIndex, height, mapElement);
			break;

		case 4:
		case 8:
			dodgems_paint_sub_fence_top_left(rideIndex, height, mapElement);
			break;

		case 7:
		case 11:
			dodgems_paint_sub_fence_bottom_right(rideIndex, height, mapElement);
			break;

		case 12:
			dodgems_paint_sub_fence_top_left(rideIndex, height, mapElement);
			dodgems_paint_sub_fence_bottom_left(rideIndex, height, mapElement);
			break;

		case 13:
		case 14:
			dodgems_paint_sub_fence_bottom_left(rideIndex, height, mapElement);
			break;

		case 15:
			dodgems_paint_sub_fence_bottom_right(rideIndex, height, mapElement);
			dodgems_paint_sub_fence_bottom_left(rideIndex, height, mapElement);
			break;
	}

	height += 30;
	if ((trackSequence / 4) & 1) {
		dodgems_paint_sub_roof(height, 0);
	} else {
		dodgems_paint_sub_roof(height, 2);
	}

	height += 6;
	for (int i = 0; i < 9; ++i) {
		RCT2_GLOBAL(0x141E9B4 + i * 4, uint16) = height;
		RCT2_GLOBAL(0x141E9B6 + i * 4, uint8) = 0x20;
	}

	height += 12;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 0x20;
	}
}

static void dodgems_paint_setup_rot_1(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	dodgems_paint_sub_floor(direction, height);

	switch (trackSequence) {
		case 0:
			dodgems_paint_sub_fence_top_right(rideIndex, height, mapElement);
			dodgems_paint_sub_fence_bottom_right(rideIndex, height, mapElement);
			break;

		case 1:
		case 2:
			dodgems_paint_sub_fence_bottom_right(rideIndex, height, mapElement);
			break;

		case 3:
			dodgems_paint_sub_fence_bottom_right(rideIndex, height, mapElement);
			dodgems_paint_sub_fence_bottom_left(rideIndex, height, mapElement);
			break;

		case 4:
		case 8:
			dodgems_paint_sub_fence_top_right(rideIndex, height, mapElement);
			break;

		case 7:
		case 11:
			dodgems_paint_sub_fence_bottom_left(rideIndex, height, mapElement);
			break;

		case 12:
			dodgems_paint_sub_fence_top_left(rideIndex, height, mapElement);
			dodgems_paint_sub_fence_top_right(rideIndex, height, mapElement);
			break;

		case 13:
		case 14:
			dodgems_paint_sub_fence_top_left(rideIndex, height, mapElement);
			break;

		case 15:
			dodgems_paint_sub_fence_top_left(rideIndex, height, mapElement);
			dodgems_paint_sub_fence_bottom_left(rideIndex, height, mapElement);
			break;
	}

	height += 30;
	if ((trackSequence / 4) & 1) {
		dodgems_paint_sub_roof(height, 1);
	} else {
		dodgems_paint_sub_roof(height, 3);
	}

	height += 6;
	for (int i = 0; i < 9; ++i) {
		RCT2_GLOBAL(0x141E9B4 + i * 4, uint16) = height;
		RCT2_GLOBAL(0x141E9B6 + i * 4, uint8) = 0x20;
	}

	height += 12;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 0x20;
	}
}

static void dodgems_paint_setup_rot_2(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	dodgems_paint_setup_rot_0(rideIndex, 15 - trackSequence, direction, height, mapElement);
}

static void dodgems_paint_setup_rot_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	dodgems_paint_setup_rot_1(rideIndex, 15 - trackSequence, direction, height, mapElement);
}

/**
 * rct2:
 */
TRACK_PAINT_FUNCTION get_track_paint_function_dodgems(int trackType, int direction) {
	if (trackType != 111) {
		return NULL;
	}

	switch (direction) {
		case 0: return dodgems_paint_setup_rot_0;
		case 1: return dodgems_paint_setup_rot_1;
		case 2: return dodgems_paint_setup_rot_2;
		case 3: return dodgems_paint_setup_rot_3;
	}

	return NULL;
}
