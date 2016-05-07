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

/* rct2: 0x007485D8 (same as loc_7667AC) */
static rct_xy16 dword_7485D8[] = {
	{.x = -1, .y = 0},
	{.x = 0, .y = -1},
	{.x = 1, .y = 0},
	{.x = 0, .y = 1},
};

/* rct2: 0x007485D8 (same as loc_7667AC) */
static rct_xy16 dword_7485da[] = {
	{.x = 0, .y = -1},
	{.x = 1, .y = 0},
	{.x = 0, .y = 1},
	{.x = -1, .y = 0},
};

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
	SPIRAL_SLIDE_LEFT_R0 = 20548,
	SPIRAL_SLIDE_CENTER_R0 = 20549,
	SPIRAL_SLIDE_RIGHT_R0 = 20550,

	SPIRAL_SLIDE_LEFT_R1 = 20551,
	SPIRAL_SLIDE_CENTER_R1 = 20552,
	SPIRAL_SLIDE_RIGHT_R1 = 20553,

	SPIRAL_SLIDE_LEFT_R2 = 20554,
	SPIRAL_SLIDE_CENTER_R2 = 20555,
	SPIRAL_SLIDE_RIGHT_R2 = 20556,

	SPIRAL_SLIDE_LEFT_R3 = 20557,
	SPIRAL_SLIDE_CENTER_R3 = 20558,
	SPIRAL_SLIDE_RIGHT_R3 = 20559,

	SPIRAL_SLIDE_INSIDE_R1 = 20560,
	SPIRAL_SLIDE_INSIDE_R2 = 20561,

	SPIRAL_SLIDE_BASE_A = 20562,
	SPIRAL_SLIDE_BASE_B = 20563,

	SPIRAL_SLIDE_FENCE_TOP_RIGHT = 20564,
	SPIRAL_SLIDE_FENCE_BOTTOM_RIGHT = 20565,
	SPIRAL_SLIDE_FENCE_BOTTOM_LEFT = 20566,
	SPIRAL_SLIDE_FENCE_TOP_LEFT = 20567,

	SPIRAL_SLIDE_PEEP = 20568, // 46 sprites per direction
};

static void spiral_slide_paint_tile_back(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	uint32 image_id = RCT2_GLOBAL(0x00F441A0, uint32);
	wooden_a_supports_paint_setup(direction & 1, 0, height, image_id, NULL);

	// Base
	image_id = ((direction & 1) ? SPIRAL_SLIDE_BASE_B : SPIRAL_SLIDE_BASE_A) | RCT2_GLOBAL(0x00F4419C, uint32);
	sub_98197C(image_id, 0, 0, 32, 32, 1, height, 0, 0, height, get_current_rotation());

	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);
	uint16 entranceLoc =
		((x / 32) + loc_7667AE[get_current_rotation()].x) |
		(((y / 32) + loc_7667AE[get_current_rotation()].y) << 8);

	uint8 entranceId = (mapElement->properties.track.sequence & 0x70) >> 4;
	rct_ride *ride = get_ride(rideIndex);

	if (ride->entrances[entranceId] != entranceLoc && ride->exits[entranceId] != entranceLoc) {
		image_id = SPIRAL_SLIDE_FENCE_TOP_LEFT | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98199C(image_id, 0, 0, 32, 1, 7, height, 0, 2, height + 2, get_current_rotation());
	}

	entranceLoc =
		((x / 32) + loc_7667AC[get_current_rotation()].x) |
		(((y / 32) + loc_7667AC[get_current_rotation()].y) << 8);

	if (ride->entrances[entranceId] != entranceLoc && ride->exits[entranceId] != entranceLoc) {
		image_id = SPIRAL_SLIDE_FENCE_TOP_RIGHT | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98199C(image_id, 0, 0, 1, 32, 7, height, 2, 0, height + 2, get_current_rotation());
	}

	RCT2_GLOBAL(0x141E9B4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9BC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;

	height += 128;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 32;
	}
}

static void spiral_slide_paint_tile_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	uint32 image_id = RCT2_GLOBAL(0x00F441A0, uint32);
	wooden_a_supports_paint_setup(direction & 1, 0, height, image_id, NULL);

	// Base
	image_id = ((direction & 1) ? SPIRAL_SLIDE_BASE_B : SPIRAL_SLIDE_BASE_A) | RCT2_GLOBAL(0x00F4419C, uint32);
	sub_98197C(image_id, 0, 0, 32, 32, 1, height, 0, 0, height, get_current_rotation());

	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);
	uint16 entranceLoc =
		((x / 32) + dword_7485D8[get_current_rotation()].x) |
		(((y / 32) + dword_7485D8[get_current_rotation()].y) << 8);

	uint8 entranceId = (mapElement->properties.track.sequence & 0x70) >> 4;
	rct_ride *ride = get_ride(rideIndex);

	if (ride->entrances[entranceId] != entranceLoc && ride->exits[entranceId] != entranceLoc) {
		image_id = SPIRAL_SLIDE_FENCE_TOP_RIGHT | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98199C(image_id, 0, 0, 1, 32, 7, height, 2, 0, height + 2, 0);
	}

	image_id = SPIRAL_SLIDE_FENCE_BOTTOM_RIGHT | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(image_id, 0, 0, 32, 1, 7, height, 0, 30, height + 2, get_current_rotation()); //TODO 1, 32

	if (direction == 0) image_id = SPIRAL_SLIDE_RIGHT_R0 | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 1) image_id = SPIRAL_SLIDE_RIGHT_R1 | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 2) image_id = SPIRAL_SLIDE_RIGHT_R2 | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 3) image_id = SPIRAL_SLIDE_RIGHT_R3 | RCT2_GLOBAL(0x00F44198, uint32);

	sub_98197C(image_id, 16, 16, 16, 16, 108, height, 16, 0, height + 3, get_current_rotation());

	RCT2_GLOBAL(0x141E9B4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9BC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;

	height += 128;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 32;
	}
}

static void spiral_slide_paint_tile_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	uint32 image_id = RCT2_GLOBAL(0x00F441A0, uint32);
	wooden_a_supports_paint_setup(direction & 1, 0, height, image_id, NULL);

	// Base
	image_id = ((direction & 1) ? SPIRAL_SLIDE_BASE_B : SPIRAL_SLIDE_BASE_A) | RCT2_GLOBAL(0x00F4419C, uint32);
	sub_98197C(image_id, 0, 0, 32, 32, 1, height, 0, 0, height, get_current_rotation());

	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);
	uint16 entranceLoc =
		((x / 32) + dword_7485da[get_current_rotation()].x) |
		(((y / 32) + dword_7485da[get_current_rotation()].y) << 8);

	uint8 entranceId = (mapElement->properties.track.sequence & 0x70) >> 4;
	rct_ride *ride = get_ride(rideIndex);

	if (ride->entrances[entranceId] != entranceLoc && ride->exits[entranceId] != entranceLoc) {
		image_id = SPIRAL_SLIDE_FENCE_TOP_LEFT | RCT2_GLOBAL(0x00F44198, uint32);

		sub_98199C(image_id, 0, 0, 32, 1, 7, height, 0, 2, height + 2, 0);
	}

	// Fence
	image_id = SPIRAL_SLIDE_FENCE_BOTTOM_LEFT | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(image_id, 0, 0, 1, 32, 7, height, 30, 0, height + 2, get_current_rotation());

	if (direction == 0) image_id = SPIRAL_SLIDE_LEFT_R0 | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 1) image_id = SPIRAL_SLIDE_LEFT_R1 | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 2) image_id = SPIRAL_SLIDE_LEFT_R2 | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 3) image_id = SPIRAL_SLIDE_LEFT_R3 | RCT2_GLOBAL(0x00F44198, uint32);

	sub_98197C(image_id, 16, 16, 16, 16, 108, height, 0, 16, height + 3, get_current_rotation());

	RCT2_GLOBAL(0x141E9B4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9BC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;

	height += 128;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 32;
	}
}

static void spiral_slide_paint_tile_front(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	uint32 image_id = RCT2_GLOBAL(0x00F441A0, uint32);
	wooden_a_supports_paint_setup(direction & 1, 0, height, image_id, NULL);

	// Base
	image_id = ((direction & 1) ? SPIRAL_SLIDE_BASE_B : SPIRAL_SLIDE_BASE_A) | RCT2_GLOBAL(0x00F4419C, uint32);
	sub_98197C(image_id, 0, 0, 32, 32, 1, height, 0, 0, height, get_current_rotation());

	// Fence 1
	image_id = SPIRAL_SLIDE_FENCE_BOTTOM_LEFT | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(image_id, 0, 0, 1, 32, 7, height, 30, 0, height + 2, get_current_rotation());

	// Fence 2
	image_id = SPIRAL_SLIDE_FENCE_BOTTOM_RIGHT | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(image_id, 0, 0, 32, 1, 7, height, 0, 30, height + 2, get_current_rotation());

	if (direction == 1) {
		image_id = SPIRAL_SLIDE_INSIDE_R1 | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(image_id, 16, 16, 2, 16, 108, height, -12, 0, height + 3, get_current_rotation());
	} else if (direction == 2) {
		image_id = SPIRAL_SLIDE_INSIDE_R2 | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(image_id, 16, 16, 16, 2, 108, height, 0, -12, height + 3, get_current_rotation());
	}


	if (direction == 0) {
		image_id = SPIRAL_SLIDE_CENTER_R0 | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(image_id, 16, 16, 16, 8, 108, height, 0, 8, height + 3, get_current_rotation());
	} else if (direction == 1) {
		image_id = SPIRAL_SLIDE_CENTER_R1 | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(image_id, 16, 16, 2, 16, 108, height, 14, 0, height + 3, get_current_rotation());
	} else if (direction == 2) {
		image_id = SPIRAL_SLIDE_CENTER_R2 | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(image_id, 16, 16, 16, 2, 108, height, 0, 14, height + 3, get_current_rotation());
	} else if (direction == 3) {
		image_id = SPIRAL_SLIDE_CENTER_R3 | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(image_id, 16, 16, 8, 16, 108, height, 8, 0, height + 3, get_current_rotation());
	}

	rct_drawpixelinfo *dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
	rct_ride *ride = get_ride(rideIndex);
	if (dpi->zoom_level == 0 && ride->slide_in_use != 0) {
		uint8 slide_progress = ride->spiral_slide_progress;
		if (slide_progress != 0) {
			slide_progress--;
		}

		if (slide_progress == 46) {
			slide_progress--;
		}

		if (slide_progress < 46) {

			int offset = SPIRAL_SLIDE_PEEP + 46 * direction;
			rct_xyz8 boundingBox = {
				.z = 108
			};
			rct_xyz16 boundingBoxOffset = {
				.z = height + 3
			};

			if (direction == 0) {
				boundingBoxOffset.x = 0;
				boundingBoxOffset.y = 8;
				boundingBox.y = 8;
				boundingBox.x = 16;
			} else if (direction == 1) {
				boundingBoxOffset.x = 14;
				boundingBoxOffset.y = 0;
				boundingBox.y = 2;
				boundingBox.x = 16;
			} else if (direction == 2) {
				boundingBoxOffset.x = 0;
				boundingBoxOffset.y = 14;
				boundingBox.y = 16;
				boundingBox.x = 2;
			} else if (direction == 3) {
				boundingBoxOffset.x = 8;
				boundingBoxOffset.y = 0;
				boundingBox.y = 16;
				boundingBox.x = 8;
			}

			image_id = (offset + slide_progress) | (ride->slide_peep_t_shirt_colour << 19) | (1 << 29);

			sub_98199C(image_id, 16, 16, boundingBox.x, boundingBox.y, boundingBox.z, height, boundingBoxOffset.x, boundingBoxOffset.y, boundingBoxOffset.z, get_current_rotation());
		}
	}

	height += 128;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 32;
	}
}


/**
 * rct: 0x007485E8
 */
static void spiral_slide_paint_setup_rot_0(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	switch (trackSequence) {
		case 0: spiral_slide_paint_tile_back(rideIndex, trackSequence, direction, height, mapElement); break;
		case 1: spiral_slide_paint_tile_right(rideIndex, trackSequence, direction, height, mapElement); break;
		case 2: spiral_slide_paint_tile_left(rideIndex, trackSequence, direction, height, mapElement); break;
		case 3: spiral_slide_paint_tile_front(rideIndex, trackSequence, direction, height, mapElement); break;
	}
}

/**
 * rct: 0x00748DE4
 */
static void spiral_slide_paint_setup_rot_1(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	switch (trackSequence) {
		case 0: spiral_slide_paint_tile_right(rideIndex, trackSequence, direction, height, mapElement); break;
		case 1: spiral_slide_paint_tile_front(rideIndex, trackSequence, direction, height, mapElement); break;
		case 2: spiral_slide_paint_tile_back(rideIndex, trackSequence, direction, height, mapElement); break;
		case 3: spiral_slide_paint_tile_left(rideIndex, trackSequence, direction, height, mapElement); break;
	}
}

/**
 * rct: 0x00749628
 */
static void spiral_slide_paint_setup_rot_2(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	switch (trackSequence) {
		case 0: spiral_slide_paint_tile_front(rideIndex, trackSequence, direction, height, mapElement); break;
		case 1: spiral_slide_paint_tile_left(rideIndex, trackSequence, direction, height, mapElement); break;
		case 2: spiral_slide_paint_tile_right(rideIndex, trackSequence, direction, height, mapElement); break;
		case 3: spiral_slide_paint_tile_back(rideIndex, trackSequence, direction, height, mapElement); break;
	}
}

/**
 * rct: 0x00749E6C
 */
static void spiral_slide_paint_setup_rot_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	switch (trackSequence) {
		case 0: spiral_slide_paint_tile_left(rideIndex, trackSequence, direction, height, mapElement); break;
		case 1: spiral_slide_paint_tile_back(rideIndex, trackSequence, direction, height, mapElement); break;
		case 2:	spiral_slide_paint_tile_front(rideIndex, trackSequence, direction, height, mapElement); break;
		case 3:	spiral_slide_paint_tile_right(rideIndex, trackSequence, direction, height, mapElement); break;
	}
}

/**
 * rct2: 0x0074840C
 */
TRACK_PAINT_FUNCTION get_track_paint_function_spiral_slide(int trackType, int direction) {
	if (trackType != 110) {
		return NULL;
	}

	switch (direction) {
		case 0: return spiral_slide_paint_setup_rot_0;
		case 1: return spiral_slide_paint_setup_rot_1;
		case 2: return spiral_slide_paint_setup_rot_2;
		case 3: return spiral_slide_paint_setup_rot_3;
	}

	return NULL;
}
