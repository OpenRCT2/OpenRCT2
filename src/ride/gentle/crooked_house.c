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

static void crooked_house_paint_supports(uint8 direction, int height) {
	wooden_a_supports_paint_setup((direction & 1), 0, height, RCT2_GLOBAL(0x00F441A0, uint32), NULL);
}

static void crooked_house_paint_floor(int height, int floor_sprite_id) {
	uint32 image_id = floor_sprite_id | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(image_id, 0, 0, 31, 31, 1, height, 0, 0, height, get_current_rotation());
}

static void crooked_house_paint_support_heights() {
	RCT2_GLOBAL(0x0141E9D0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9CC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9BC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9B4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9C0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9D4, uint16) = 0xFFFF;
}

enum {
	SEGMENT_B4 = (1 << 0),
	SEGMENT_B8 = (1 << 1),
	SEGMENT_BC = (1 << 2),
	SEGMENT_C0 = (1 << 3),
	SEGMENT_C4 = (1 << 4),
	SEGMENT_C8 = (1 << 5),
	SEGMENT_CC = (1 << 6),
	SEGMENT_D0 = (1 << 7),
	SEGMENT_D4 = (1 << 8),
};

static void crooked_house_paint_support_height(int flags, uint16 height, uint8 segment_flags) {
	for (int s = 0; s < 9; s++) {
		if (flags & (1 << s)) {
			RCT2_GLOBAL(0x0141E9B4 + s * 4, uint16) = height;
			RCT2_GLOBAL(0x0141E9B6 + s * 4, uint8) = segment_flags;
		}
	}
}

static void crooked_house_paint_max_height(int height) {
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x00141E9DA, uint8) = 0x20;
	}
}

static void crooked_house_fence_top_left(rct_ride *ride, int height, rct_map_element *mapElement) {
	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);
	uint8 entranceId = (mapElement->properties.entrance.index & 0x70) >> 4;

	uint16 entranceLoc =
		((x / 32) + loc_7667AE[get_current_rotation()].x) |
		(((y / 32) + loc_7667AE[get_current_rotation()].y) << 8);

	if (ride->entrances[entranceId] != entranceLoc && ride->exits[entranceId] != entranceLoc) {
		uint32 image_id = 22141 | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98199C(image_id, 0, 0, 32, 1, 7, height, 0, 2, height + 2, get_current_rotation());
	}
}

static void crooked_house_fence_top_right(rct_ride *ride, int height, rct_map_element *mapElement) {
	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);
	uint8 entranceId = (mapElement->properties.entrance.index & 0x70) >> 4;

	uint16 entranceLoc =
		((x / 32) + loc_7667AC[get_current_rotation()].x) |
		(((y / 32) + loc_7667AC[get_current_rotation()].y) << 8);

	if (ride->entrances[entranceId] != entranceLoc && ride->exits[entranceId] != entranceLoc) {
		uint32 image_id = 22138 | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98199C(image_id, 0, 0, 1, 32, 7, height, 0, 2, height + 2, get_current_rotation());
	}
}

static void crooked_house_fence_bottom_right(rct_ride *ride, int height, rct_map_element *mapElement) {
	uint32 image_id = 22139 | RCT2_GLOBAL(0x00F441A0, uint32);
	sub_98199C(image_id, 0, 0, 32, 1, 7, height, 0, 30, height + 2, get_current_rotation());
}

static void crooked_house_fence_bottom_left(rct_ride *ride, int height, rct_map_element *mapElement) {
	uint32 image_id = 22140 | RCT2_GLOBAL(0x00F441A0, uint32);
	sub_98199C(image_id, 0, 0, 1, 32, 7, height, 30, 0, height + 2, get_current_rotation());
}

typedef struct {
	sint16 offset_x;
	sint16 offset_y;
	sint16 length_x;
	sint16 length_y;
} rct_crooked_house_bound_box;

rct_crooked_house_bound_box crooked_house_data[] = {
	{6,   0,   42, 24},
	{0,   0,   0,  0},
	{-16, -16, 32, 32},
	{0,   0,   0,  0}, // unused
	{0,   6,   24, 42}
};

/**
 * rct2: 0x0088ABA4
 * @param (edi) direction
 * @param (al) al
 * @param (cl) cl
 * @param (ebx) image_id
 * @param (edx) height
 */
static void sub_88ABA4(uint8 direction, uint8 x_offset, uint8 y_offset, uint32 segment, int height) {
	//RCT2_CALLPROC_X(0x88ABA4, al, segment, cl, height, 0, direction, 0);
	//return;

	//  push    dword_9DE578
	rct_map_element *original_map_element = RCT2_GLOBAL(0x9DE578, rct_map_element*);
	//    push    edx
	RCT2_GLOBAL(0x14281B0, uint32) = segment;
	RCT2_GLOBAL(0x14281B4, uint32) = direction;

	rct_ride *ride = get_ride(original_map_element->properties.track.ride_index);

	rct_ride_entry *ride_type = get_ride_entry(ride->subtype);

	RCT2_GLOBAL(0x0014281B8, uint32) = ride_type->vehicles[0].base_image_id;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
		if (ride->vehicles[0] != -1) {
			rct_sprite *sprite = &g_sprite_list[ride->vehicles[0]];
			RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_SPRITE;
			RCT2_GLOBAL(0x9DE578, rct_sprite *) = sprite;
		}
	}


	uint32 image_id = (direction + ride_type->vehicles[0].base_image_id) | RCT2_GLOBAL(0x00F441A0, uint32);

	rct_crooked_house_bound_box boundBox = crooked_house_data[segment];
	sub_98197C(image_id, x_offset, y_offset, boundBox.length_x, boundBox.length_y, 127, height + 3, boundBox.offset_x, boundBox.offset_y, height + 3, get_current_rotation());
}

static void crooked_house_paint_setup_889F08(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	crooked_house_paint_supports(direction, height);
	crooked_house_paint_floor(height, 22137);
	crooked_house_paint_support_heights();
	crooked_house_paint_max_height(height + 128);
}

static void crooked_house_paint_setup_889FCC(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	crooked_house_paint_supports(direction, height);
	crooked_house_paint_floor(height, 22137);

	rct_ride *ride = get_ride(rideIndex);
	crooked_house_fence_top_left(ride, height, mapElement);
	crooked_house_fence_top_right(ride, height, mapElement);

	height += 2;
	crooked_house_paint_support_heights();
	crooked_house_paint_support_height(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, height, 0x20);

	crooked_house_paint_max_height(height + 126);
}

static void crooked_house_paint_setup_88A1D0(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	crooked_house_paint_supports(direction, height);
	crooked_house_paint_floor(height, 22136);

	rct_ride *ride = get_ride(rideIndex);
	crooked_house_fence_top_right(ride, height, mapElement);
	crooked_house_fence_bottom_right(ride, height, mapElement);

	sub_88ABA4(direction, 32, 224, 0, height);

	height += 2;
	crooked_house_paint_support_heights();
	crooked_house_paint_support_height(SEGMENT_CC | SEGMENT_BC | SEGMENT_D4, height, 0x20);

	crooked_house_paint_max_height(height + 126);
}

static void crooked_house_paint_setup_88A392(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	crooked_house_paint_supports(direction, height);
	crooked_house_paint_floor(height, 22135);

	rct_ride *ride = get_ride(rideIndex);
	crooked_house_fence_top_left(ride, height, mapElement);
	crooked_house_fence_bottom_left(ride, height, mapElement);

	sub_88ABA4(direction, 224, 32, 4, height);

	height += 2;
	crooked_house_paint_support_heights();
	crooked_house_paint_support_height(SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0, height, 0x20);

	crooked_house_paint_max_height(height + 126);
}

static void crooked_house_paint_setup_88A554(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	crooked_house_paint_supports(direction, height);
	crooked_house_paint_floor(height, 22134);

	rct_ride *ride = get_ride(rideIndex);
	crooked_house_fence_bottom_left(ride, height, mapElement);
	crooked_house_fence_bottom_right(ride, height, mapElement);

	sub_88ABA4(direction, 224, 224, 2, height);

	height += 2;
	crooked_house_paint_support_heights();
	crooked_house_paint_support_height(SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4, height, 0x20);

	crooked_house_paint_max_height(height + 126);
}


static void crooked_house_paint_setup_88A97C(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	crooked_house_paint_supports(direction, height);
	crooked_house_paint_floor(height, 22136);

	rct_ride *ride = get_ride(rideIndex);
	crooked_house_fence_bottom_right(ride, height, mapElement);

	sub_88ABA4(direction, 0, 224, 1, height);

	crooked_house_paint_support_heights();
	crooked_house_paint_max_height(height + 128);
}

static void crooked_house_paint_setup_88A821(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	crooked_house_paint_supports(direction, height);
	crooked_house_paint_floor(height, 22137);

	rct_ride *ride = get_ride(rideIndex);
	crooked_house_fence_top_left(ride, height, mapElement);

	crooked_house_paint_support_heights();
	crooked_house_paint_max_height(height + 128);
}

static void crooked_house_paint_setup_88A6C6(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	crooked_house_paint_supports(direction, height);
	crooked_house_paint_floor(height, 22137);

	rct_ride *ride = get_ride(rideIndex);
	crooked_house_fence_top_right(ride, height, mapElement);
	crooked_house_paint_support_heights();
	crooked_house_paint_max_height(height + 128);
}

// bottom center
static void crooked_house_paint_setup_88AA90(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	crooked_house_paint_supports(direction, height);
	crooked_house_paint_floor(height, 22135);

	rct_ride *ride = get_ride(rideIndex);
	crooked_house_fence_bottom_left(ride, height, mapElement);

	//sub_88ABA4(rideIndex, 224, 0, 3, height);

	crooked_house_paint_support_heights();
	crooked_house_paint_max_height(height + 128);
}

TRACK_PAINT_FUNCTION crooked_house_paint_setup_functions[][9] = {
	{
		crooked_house_paint_setup_889F08,
		crooked_house_paint_setup_889FCC,
		crooked_house_paint_setup_88A6C6,
		crooked_house_paint_setup_88A1D0,
		crooked_house_paint_setup_88A821,
		crooked_house_paint_setup_88A97C,
		crooked_house_paint_setup_88A392,
		crooked_house_paint_setup_88A554,
		crooked_house_paint_setup_88AA90,
	},
	{
		crooked_house_paint_setup_889F08,
		crooked_house_paint_setup_88A1D0,
		crooked_house_paint_setup_88A97C,
		crooked_house_paint_setup_88A554,
		crooked_house_paint_setup_88A6C6,
		crooked_house_paint_setup_88AA90,
		crooked_house_paint_setup_889FCC,
		crooked_house_paint_setup_88A392,
		crooked_house_paint_setup_88A821,
	},
	{
		crooked_house_paint_setup_889F08,
		crooked_house_paint_setup_88A554,
		crooked_house_paint_setup_88AA90,
		crooked_house_paint_setup_88A392,
		crooked_house_paint_setup_88A97C,
		crooked_house_paint_setup_88A821,
		crooked_house_paint_setup_88A1D0,
		crooked_house_paint_setup_889FCC,
		crooked_house_paint_setup_88A6C6,
	},
	{
		crooked_house_paint_setup_889F08,
		crooked_house_paint_setup_88A392,
		crooked_house_paint_setup_88A821,
		crooked_house_paint_setup_889FCC,
		crooked_house_paint_setup_88AA90,
		crooked_house_paint_setup_88A6C6,
		crooked_house_paint_setup_88A554,
		crooked_house_paint_setup_88A1D0,
		crooked_house_paint_setup_88A97C,
	},
};

static void crooked_house_paint_setup(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	crooked_house_paint_setup_functions[direction][trackSequence](rideIndex, trackSequence, direction, height, mapElement);
}

TRACK_PAINT_FUNCTION get_track_paint_function_crooked_house(int trackType, int direction) {
	if (trackType != 123) {
		return NULL;
	}

	return crooked_house_paint_setup;
}
