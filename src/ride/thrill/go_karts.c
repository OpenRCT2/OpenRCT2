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

#include "../../common.h"
#include "../track_paint.h"
#include "../track.h"
#include "../vehicle_paint.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"

enum
{
	SPR_GO_KARTS_FLAT_SW_NE = 20752,
	SPR_GO_KARTS_FLAT_NW_SE = 20753,
	SPR_GO_KARTS_FLAT_FRONT_SW_NE = 20754,
	SPR_GO_KARTS_FLAT_FRONT_NW_SE = 20755,

	SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SW_NE = 20772,
	SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NW_SE = 20773,
	SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NE_SW = 20774,
	SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SE_NW = 20775,
	SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SW_NE = 20776,
	SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NW_SE = 20777,
	SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NE_SW = 20778,
	SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SE_NW = 20779,
	SPR_GO_KARTS_25_DEG_UP_SW_NE = 20780,
	SPR_GO_KARTS_25_DEG_UP_NW_SE = 20781,
	SPR_GO_KARTS_25_DEG_UP_NE_SW = 20782,
	SPR_GO_KARTS_25_DEG_UP_SE_NW = 20783,
	SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 20784,
	SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 20785,
	SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 20786,
	SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 20787,
	SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 20788,
	SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 20789,
	SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 20790,
	SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 20791,
	SPR_GO_KARTS_25_DEG_UP_FRONT_SW_NE = 20792,
	SPR_GO_KARTS_25_DEG_UP_FRONT_NW_SE = 20793,
	SPR_GO_KARTS_25_DEG_UP_FRONT_NE_SW = 20794,
	SPR_GO_KARTS_25_DEG_UP_FRONT_SE_NW = 20795,
};

static const uint32 go_karts_track_pieces_25_deg_up[4][2] = {
	{SPR_GO_KARTS_25_DEG_UP_SW_NE, SPR_GO_KARTS_25_DEG_UP_FRONT_SW_NE},
	{SPR_GO_KARTS_25_DEG_UP_NW_SE, SPR_GO_KARTS_25_DEG_UP_FRONT_NW_SE},
	{SPR_GO_KARTS_25_DEG_UP_NE_SW, SPR_GO_KARTS_25_DEG_UP_FRONT_NE_SW},
	{SPR_GO_KARTS_25_DEG_UP_SE_NW, SPR_GO_KARTS_25_DEG_UP_FRONT_SE_NW},
};

static const uint32 go_karts_track_pieces_flat_to_25_deg_up[4][2] = {
	{SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SW_NE, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SW_NE},
	{SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NW_SE, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NW_SE},
	{SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NE_SW, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NE_SW},
	{SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SE_NW, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SE_NW},
};

static const uint32 go_karts_track_pieces_25_deg_up_to_flat[4][2] = {
	{SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SW_NE, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SW_NE},
	{SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NW_SE, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NW_SE},
	{SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NE_SW, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NE_SW},
	{SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SE_NW, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SE_NW},
};

/** rct2: 0x0074A748 */
static void paint_go_karts_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;
	if (direction == 0 || direction == 2) {
		imageId = SPR_GO_KARTS_FLAT_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 28, 1, height, 0, 2, height, get_current_rotation());

		imageId = SPR_GO_KARTS_FLAT_FRONT_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 1, 3, height, 0, 29, height + 2, get_current_rotation());

		paint_util_push_tunnel_left(height, TUNNEL_0);
	} else {
		imageId = SPR_GO_KARTS_FLAT_NW_SE | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 28, 32, 1, height, 2, 0, height, get_current_rotation());

		imageId = SPR_GO_KARTS_FLAT_FRONT_NW_SE | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 1, 32, 3, height, 29, 0, height + 2, get_current_rotation());

		paint_util_push_tunnel_right(height, TUNNEL_0);
	}

	wooden_a_supports_paint_setup((direction & 1), 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0074A758 */
static void paint_go_karts_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;
	paint_struct * ps;

	imageId = go_karts_track_pieces_25_deg_up[direction][0] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 0 || direction == 2) {
		ps = sub_98197C(imageId, 0, 0, 32, 28, 1, height, 0, 2, height, get_current_rotation());
	} else {
		ps = sub_98197C(imageId, 0, 0, 28, 32, 1, height, 2, 0, height, get_current_rotation());
	}

	if (direction == 1 || direction == 2) {
		RCT2_GLOBAL(0x009DEA58, paint_struct*) = ps;
	}

	imageId = go_karts_track_pieces_25_deg_up[direction][1] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 0 || direction == 2) {
		sub_98197C(imageId, 0, 0, 32, 1, 11, height, 0, 29, height + 2, get_current_rotation());
	} else {
		sub_98197C(imageId, 0, 0, 1, 32, 1, height, 29, 0, height + 2, get_current_rotation());
	}

	switch (direction) {
		case 0:
			wooden_a_supports_paint_setup(0, 9, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height - 8, TUNNEL_7);
			break;
		case 1:
			wooden_a_supports_paint_setup(1, 10, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height + 8, TUNNEL_8);
			break;
		case 2:
			wooden_a_supports_paint_setup(0, 11, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height + 8, TUNNEL_8);
			break;
		case 3:
			wooden_a_supports_paint_setup(1, 12, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height - 8, TUNNEL_7);
			break;
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x0074A768 */
static void paint_go_karts_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;
	paint_struct * ps;

	imageId = go_karts_track_pieces_flat_to_25_deg_up[direction][0] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 0 || direction == 2) {
		ps = sub_98197C(imageId, 0, 0, 32, 28, 1, height, 0, 2, height, get_current_rotation());
	} else {
		ps = sub_98197C(imageId, 0, 0, 28, 32, 1, height, 2, 0, height, get_current_rotation());
	}

	if (direction == 1 || direction == 2) {
		RCT2_GLOBAL(0x009DEA58, paint_struct*) = ps;
	}

	imageId = go_karts_track_pieces_flat_to_25_deg_up[direction][1] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 0 || direction == 2) {
		sub_98197C(imageId, 0, 0, 32, 1, 11, height, 0, 29, height + 2, get_current_rotation());
	} else {
		sub_98197C(imageId, 0, 0, 1, 32, 1, height, 29, 0, height + 2, get_current_rotation());
	}

	switch (direction) {
		case 0:
			wooden_a_supports_paint_setup(0, 1, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		case 1:
			wooden_a_supports_paint_setup(1, 2, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height, TUNNEL_8);
			break;
		case 2:
			wooden_a_supports_paint_setup(0, 3, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height, TUNNEL_8);
			break;
		case 3:
			wooden_a_supports_paint_setup(1, 4, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x */
static void paint_go_karts_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;
	paint_struct * ps;

	imageId = go_karts_track_pieces_25_deg_up_to_flat[direction][0] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 0 || direction == 2) {
		ps = sub_98197C(imageId, 0, 0, 32, 28, 1, height, 0, 2, height, get_current_rotation());
	} else {
		ps = sub_98197C(imageId, 0, 0, 28, 32, 1, height, 2, 0, height, get_current_rotation());
	}

	if (direction == 1 || direction == 2) {
		RCT2_GLOBAL(0x009DEA58, paint_struct*) = ps;
	}

	imageId = go_karts_track_pieces_25_deg_up_to_flat[direction][1] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction == 0 || direction == 2) {
		sub_98197C(imageId, 0, 0, 32, 1, 11, height, 0, 29, height + 2, get_current_rotation());
	} else {
		sub_98197C(imageId, 0, 0, 1, 32, 1, height, 29, 0, height + 2, get_current_rotation());
	}

	switch (direction) {
		case 0:
			wooden_a_supports_paint_setup(0, 5, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height - 8, TUNNEL_6);
			break;
		case 1:
			wooden_a_supports_paint_setup(1, 6, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height + 8, TUNNEL_14);
			break;
		case 2:
			wooden_a_supports_paint_setup(0, 7, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height + 8, TUNNEL_14);
			break;
		case 3:
			wooden_a_supports_paint_setup(1, 8, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height - 8, TUNNEL_6);
			break;
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x0074A788 */
static void paint_go_karts_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	paint_go_karts_track_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x0074A798 */
static void paint_go_karts_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	paint_go_karts_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x0074A7A8 */
static void paint_go_karts_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	paint_go_karts_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x */
static void paint_go_karts_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_go_karts_track_left_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_go_karts_track_right_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/**
 * rct2: 0x0074A668
 */
TRACK_PAINT_FUNCTION get_track_paint_function_go_karts(int trackType, int direction)
{
	switch (trackType) {
		case TRACK_ELEM_FLAT:
			return paint_go_karts_track_flat;

		case TRACK_ELEM_END_STATION:
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
			return paint_go_karts_station;

		case TRACK_ELEM_25_DEG_UP:
			return paint_go_karts_track_25_deg_up;
		case TRACK_ELEM_FLAT_TO_25_DEG_UP:
			return paint_go_karts_track_flat_to_25_deg_up;
		case TRACK_ELEM_25_DEG_UP_TO_FLAT:
			return paint_go_karts_track_25_deg_up_to_flat;

		case TRACK_ELEM_25_DEG_DOWN:
			return paint_go_karts_track_25_deg_down;
		case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
			return paint_go_karts_track_flat_to_25_deg_down;
		case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
			return paint_go_karts_track_25_deg_down_to_flat;

		case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
			return paint_go_karts_track_left_quarter_turn_1_tile;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
			return paint_go_karts_track_right_quarter_turn_1_tile;
	}

	return NULL;
}
