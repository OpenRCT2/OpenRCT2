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

#include "../track_paint.h"
#include "../track.h"
#include "../vehicle_paint.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../ride_data.h"
#include "../../world/map.h"

enum
{
	SPR_GHOST_TRAIN_TRACK_FLAT_SW_NE = 28821,
	SPR_GHOST_TRAIN_TRACK_FLAT_NW_SE = 28822,
	SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_SW_NE = 28823,
	SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_NW_SE = 28824,
	SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_NE_SW = 28825,
	SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_SE_NW = 28826,

	SPR_GHOST_TRAIN_TRACK_25_DEG_UP_SW_NE = 28831,
	SPR_GHOST_TRAIN_TRACK_25_DEG_UP_NW_SE = 28832,
	SPR_GHOST_TRAIN_TRACK_25_DEG_UP_NE_SW = 28833,
	SPR_GHOST_TRAIN_TRACK_25_DEG_UP_SE_NW = 28834,
	SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 28835,
	SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 28836,
	SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 28837,
	SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 28838,

	SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_SW_NE = 28843,
	SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_NW_SE = 28844,
	SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_NE_SW = 28845,
	SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_SE_NW = 28846,

	SPR_GHOST_TRAIN_TRACK_BRAKES_SW_NE = 28881,
	SPR_GHOST_TRAIN_TRACK_BRAKES_NW_SE = 28882
};

static const uint32 ghost_train_track_pieces_flat[4] = {
	SPR_GHOST_TRAIN_TRACK_FLAT_SW_NE,
	SPR_GHOST_TRAIN_TRACK_FLAT_NW_SE,
	SPR_GHOST_TRAIN_TRACK_FLAT_SW_NE,
	SPR_GHOST_TRAIN_TRACK_FLAT_NW_SE,
};

static const uint32 ghost_train_track_pieces_flat_to_25_deg_up[4][2] = {
	{SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_SW_NE, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_SW_NE},
	{SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_NW_SE, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_NW_SE},
	{SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_NE_SW, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_NE_SW},
	{SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_SE_NW, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_SE_NW},
};

static const uint32 ghost_train_track_pieces_25_deg_up[4][2] = {
	{SPR_GHOST_TRAIN_TRACK_25_DEG_UP_SW_NE, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_SW_NE},
	{SPR_GHOST_TRAIN_TRACK_25_DEG_UP_NW_SE, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_NW_SE},
	{SPR_GHOST_TRAIN_TRACK_25_DEG_UP_NE_SW, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_NE_SW},
	{SPR_GHOST_TRAIN_TRACK_25_DEG_UP_SE_NW, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_SE_NW},
};

static const uint32 ghost_train_track_pieces_brakes[4] = {
	SPR_GHOST_TRAIN_TRACK_BRAKES_SW_NE,
	SPR_GHOST_TRAIN_TRACK_BRAKES_NW_SE,
	SPR_GHOST_TRAIN_TRACK_BRAKES_SW_NE,
	SPR_GHOST_TRAIN_TRACK_BRAKES_NW_SE,
};

/** rct2: 0x00770BEC */
static void paint_ghost_train_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_xy16 position = {RCT2_GLOBAL(0x009DE56A, sint16), RCT2_GLOBAL(0x009DE56E, sint16)};

	uint32 imageId = ghost_train_track_pieces_flat[direction] | RCT2_GLOBAL(0x00F44198, uint32);

	if (direction == 0 || direction == 2) {
		sub_98197C(imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
	} else {
		sub_98197C(imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
	}

	if (direction == 0 || direction == 2) {
		paint_util_push_tunnel_left(height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_right(height, TUNNEL_0);
	}

	if (track_paint_util_should_paint_supports(position)) {
		metal_a_supports_paint_setup(3, 4, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x00770BFC */
static void paint_ghost_train_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_xy16 position = {RCT2_GLOBAL(0x009DE56A, sint16), RCT2_GLOBAL(0x009DE56E, sint16)};

	uint32 imageId = ghost_train_track_pieces_25_deg_up[direction][0] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 0 || direction == 2) {
		sub_98197C(imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
	} else {
		sub_98197C(imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
	}

	imageId = ghost_train_track_pieces_25_deg_up[direction][1] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 0 || direction == 2) {
		sub_98197C(imageId, 0, 0, 32, 1, 23, height, 0, 27, height, get_current_rotation());
	} else {
		sub_98197C(imageId, 0, 0, 1, 32, 23, height, 27, 0, height, get_current_rotation());
	}

	if (track_paint_util_should_paint_supports(position)) {
		metal_a_supports_paint_setup(3, 4, 8, height, RCT2_GLOBAL(0x00F4419C, uint32));
	}


	switch (direction) {
		case 0:
			paint_util_push_tunnel_left(height - 8, TUNNEL_1);
			break;
		case 1:
			paint_util_push_tunnel_right(height + 8, TUNNEL_2);
			break;
		case 2:
			paint_util_push_tunnel_left(height + 8, TUNNEL_2);
			break;
		case 3:
			paint_util_push_tunnel_right(height - 8, TUNNEL_1);
			break;
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x00770C0C */
static void paint_ghost_train_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_xy16 position = {RCT2_GLOBAL(0x009DE56A, sint16), RCT2_GLOBAL(0x009DE56E, sint16)};

	uint32 imageId = ghost_train_track_pieces_flat_to_25_deg_up[direction][0] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 0 || direction == 2) {
		sub_98197C(imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
	} else {
		sub_98197C(imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
	}

	imageId = ghost_train_track_pieces_flat_to_25_deg_up[direction][1] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 0 || direction == 2) {
		sub_98197C(imageId, 0, 0, 32, 1, 15, height, 0, 27, height, get_current_rotation());
	} else {
		sub_98197C(imageId, 0, 0, 1, 32, 15, height, 27, 0, height, get_current_rotation());
	}

	if (track_paint_util_should_paint_supports(position)) {
		metal_a_supports_paint_setup(3, 4, 3, height, RCT2_GLOBAL(0x00F4419C, uint32));
	}

	switch (direction) {
		case 0:
			paint_util_push_tunnel_left(height, TUNNEL_0);
			break;
		case 1:
			paint_util_push_tunnel_right(height, TUNNEL_2);
			break;
		case 2:
			paint_util_push_tunnel_left(height, TUNNEL_2);
			break;
		case 3:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x */
static void paint_ghost_train_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_ghost_train_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_ghost_train_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_ghost_train_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_ghost_train_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_ghost_train_track_right_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_ghost_train_track_left_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_ghost_train_track_left_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_ghost_train_track_right_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_ghost_train_track_spinning_tunnel(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x00770CDC */
static void paint_ghost_train_track_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_xy16 position = {RCT2_GLOBAL(0x009DE56A, sint16), RCT2_GLOBAL(0x009DE56E, sint16)};

	uint32 imageId = ghost_train_track_pieces_brakes[direction] | RCT2_GLOBAL(0x00F44198, uint32);

	if (direction == 0 || direction == 2) {
		sub_98197C(imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
	} else {
		sub_98197C(imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
	}

	if (direction == 0 || direction == 2) {
		paint_util_push_tunnel_left(height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_right(height, TUNNEL_0);
	}

	if (track_paint_util_should_paint_supports(position)) {
		metal_a_supports_paint_setup(3, 4, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/**
 * rct2: 0x00770924
 */
TRACK_PAINT_FUNCTION get_track_paint_function_ghost_train(int trackType, int direction) {
	switch(trackType) {
		case TRACK_ELEM_FLAT:
			return paint_ghost_train_track_flat;

		case TRACK_ELEM_END_STATION:
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
			return paint_ghost_train_station;

		case TRACK_ELEM_25_DEG_UP:
			return paint_ghost_train_track_25_deg_up;
		case TRACK_ELEM_FLAT_TO_25_DEG_UP:
			return paint_ghost_train_track_flat_to_25_deg_up;
		case TRACK_ELEM_25_DEG_UP_TO_FLAT:
			return paint_ghost_train_track_25_deg_up_to_flat;

		case TRACK_ELEM_25_DEG_DOWN:
			return paint_ghost_train_track_25_deg_down;
		case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
			return paint_ghost_train_track_flat_to_25_deg_down;
		case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
			return paint_ghost_train_track_25_deg_down_to_flat;

		case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
			return paint_ghost_train_track_left_quarter_turn_3_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
			return paint_ghost_train_track_right_quarter_turn_3_tiles;

		case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
			return paint_ghost_train_track_left_quarter_turn_1_tile;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
			return paint_ghost_train_track_right_quarter_turn_1_tile;

		case TRACK_ELEM_BRAKES:
			return paint_ghost_train_track_brakes;

		case TRACK_ELEM_SPINNING_TUNNEL:
			return paint_ghost_train_track_spinning_tunnel;
	}

	return NULL;
}
