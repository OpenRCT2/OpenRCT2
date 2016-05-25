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
#include "../ride_data.h"
#include "../../world/map.h"

enum
{
	SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_0 = 23293,
	SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_1 = 23294,
	SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_2 = 23295,
	SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_3 = 23296,
	SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_0 = 23297,
	SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_1 = 23298,
	SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_2 = 23299,
	SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_3 = 23300,
	SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_0 = 23301,
	SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_1 = 23302,
	SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_2 = 23303,
	SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_3 = 23304,
	SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_0 = 23305,
	SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_1 = 23306,
	SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_2 = 23307,
	SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_3 = 23308,
	SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_0 = 23309,
	SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_1 = 23310,
	SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_2 = 23311,
	SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_3 = 23312,
	SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_0 = 23313,
	SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_1 = 23314,
	SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_2 = 23315,
	SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_3 = 23316,
	SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_0 = 23317,
	SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_1 = 23318,
	SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_2 = 23319,
	SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_3 = 23320,
	SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_0 = 23321,
	SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_1 = 23322,
	SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_2 = 23323,
	SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_3 = 23324,
};

static const uint32 ghost_train_track_pieces_right_eight_to_diag[4][4] = {
	{
		SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_0,
		SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_1,
		SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_2,
		SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_3,
	},
	{
		SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_0,
		SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_1,
		SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_2,
		SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_3,
	},
	{
		SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_0,
		SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_1,
		SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_2,
		SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_3,
	},
	{
		SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_0,
		SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_1,
		SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_2,
		SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_3,
	},
};

static const rct_xy16 ghost_train_track_pieces_right_eight_to_diag_bounds[4][4] = {
	{
		{32, 20},
		{32, 16},
		{16, 16},
		{16, 16},
	},
	{
		{20, 32},
		{16, 32},
		{16, 16},
		{16, 16},
	},
	{
		{32, 20},
		{34, 16},
		{28, 28},
		{16, 18},
	},
	{
		{20, 32},
		{16, 32},
		{16, 16},
		{16, 16},
	},
};

static const rct_xy16 ghost_train_track_pieces_right_eight_to_diag_offset[4][4] = {
	{
		{0, 6},
		{0,  16},
		{0,  0},
		{16, 0},
	},
	{
		{6, 0},
		{16, 0},
		{0,  16},
		{0,  0},
	},
	{
		{0, 6},
		{0,  0},
		{4,  4},
		{0,  16},
	},
	{
		{6, 0},
		{0,  0},
		{16, 0},
		{16, 16},
	},
};

static const uint32 ghost_train_track_pieces_left_eight_to_diag[4][4] = {
	{
		SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_0,
		SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_1,
		SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_2,
		SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_3,
	},
	{
		SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_0,
		SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_1,
		SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_2,
		SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_3,
	},
	{
		SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_0,
		SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_1,
		SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_2,
		SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_3,
	},
	{
		SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_0,
		SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_1,
		SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_2,
		SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_3,
	},
};

static const rct_xy16 ghost_train_track_pieces_left_eight_to_diag_bounds[4][4] = {
	{
		{32, 20},
		{32, 16},
		{16, 16},
		{16, 16},
	},
	{
		{20, 32},
		{16, 34},
		{16, 16},
		{18, 16},
	},
	{
		{32, 20},
		{32, 16},
		{16, 16},
		{16, 16},
	},
	{
		{20, 32},
		{16, 32},
		{16, 16},
		{16, 16},
	},
};

static const rct_xy16 ghost_train_track_pieces_left_eight_to_diag_offset[4][4] = {
	{
		{0, 6},
		{0,  0},
		{0,  16},
		{16, 16},
	},
	{
		{6, 0},
		{0,  0},
		{16, 16},
		{16, 0},
	},
	{
		{0, 6},
		{0,  16},
		{16, 0},
		{0,  0},
	},
	{
		{6, 0},
		{16, 0},
		{0,  0},
		{0,  16},
	},
};

/** rct2: 0x */
static void paint_monorail_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_right_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_left_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_right_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_left_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

static const sint8 paint_monorail_eighth_to_diag_index[] = {0, 1, 2, -1, 3};

/** rct2: 0x008AE31C */
static void paint_monorail_track_left_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;
	sint8 index = paint_monorail_eighth_to_diag_index[trackSequence];
	if (index >= 0) {
		imageId = ghost_train_track_pieces_left_eight_to_diag[direction][index] | RCT2_GLOBAL(0x00F44198, uint32);
		const rct_xy16 offset = ghost_train_track_pieces_left_eight_to_diag_offset[direction][index];
		const rct_xy16 bounds = ghost_train_track_pieces_left_eight_to_diag_bounds[direction][index];
		sub_98197C(imageId, 0, 0, bounds.x, bounds.y, 2, height, offset.x, offset.y, height, get_current_rotation());
	}

	switch (trackSequence) {
		case 0:
			metal_a_supports_paint_setup(3, 4, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			break;
		case 4:
			if (direction == 0) metal_a_supports_paint_setup(3, 3, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			if (direction == 1) metal_a_supports_paint_setup(3, 1, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			if (direction == 2) metal_a_supports_paint_setup(3, 0, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			if (direction == 3) metal_a_supports_paint_setup(3, 2, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			break;
	}

	if (direction == 0 && trackSequence == 0) {
		paint_util_push_tunnel_left(height, TUNNEL_0);
	}
	if (direction == 3 && trackSequence == 0) {
		paint_util_push_tunnel_right(height, TUNNEL_0);
	}

	int blockedSegments;
	switch (trackSequence) {
		case 0: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC; break;
		case 1: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4; break;
		case 2: blockedSegments = SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
		case 3: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_C8; break;
		case 4: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C8 | SEGMENT_C0 | SEGMENT_D4; break;
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AE32C */
static void paint_monorail_track_right_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;
	sint8 index = paint_monorail_eighth_to_diag_index[trackSequence];
	if (index >= 0) {
		imageId = ghost_train_track_pieces_right_eight_to_diag[direction][index] | RCT2_GLOBAL(0x00F44198, uint32);
		const rct_xy16 offset = ghost_train_track_pieces_right_eight_to_diag_offset[direction][index];
		const rct_xy16 bounds = ghost_train_track_pieces_right_eight_to_diag_bounds[direction][index];
		sub_98197C(imageId, 0, 0, bounds.x, bounds.y, 2, height, offset.x, offset.y, height, get_current_rotation());
	}

	switch (trackSequence) {
		case 0:
			metal_a_supports_paint_setup(3, 4, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			break;
		case 4:
			if (direction == 0) metal_a_supports_paint_setup(3, 1, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			if (direction == 1) metal_a_supports_paint_setup(3, 0, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			if (direction == 2) metal_a_supports_paint_setup(3, 2, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			if (direction == 3) metal_a_supports_paint_setup(3, 3, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			break;
	}

	if (direction == 0 && trackSequence == 0) {
		paint_util_push_tunnel_left(height, TUNNEL_0);
	}
	if (direction == 3 && trackSequence == 0) {
		paint_util_push_tunnel_right(height, TUNNEL_0);
	}

	int blockedSegments;
	switch (trackSequence) {
		case 0: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC; break;
		case 1: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
		case 2: blockedSegments = SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4; break;
		case 3: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_C0 | SEGMENT_D4; break;
		case 4: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_D4; break;
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AE33C */
static void paint_monorail_track_left_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	const uint8 map[] = {4, 2, 3, 1, 0};
	trackSequence = map[trackSequence];
	paint_monorail_track_right_eighth_to_diag(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AE34C */
static void paint_monorail_track_right_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	const uint8 map[] = {4, 2, 3, 1, 0};
	trackSequence = map[trackSequence];
	paint_monorail_track_left_eighth_to_diag(rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/** rct2: 0x */
static void paint_monorail_track_diag_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_diag_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_diag_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_diag_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_diag_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_diag_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_monorail_track_diag_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/**
 * rct2: 0x008ADF34
 */
TRACK_PAINT_FUNCTION get_track_paint_function_monorail(int trackType, int direction)
{
	switch (trackType) {
		case TRACK_ELEM_FLAT:
			return paint_monorail_track_flat;

		case TRACK_ELEM_END_STATION:
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
			return paint_monorail_station;

		case TRACK_ELEM_25_DEG_UP:
			return paint_monorail_track_25_deg_up;
		case TRACK_ELEM_FLAT_TO_25_DEG_UP:
			return paint_monorail_track_flat_to_25_deg_up;
		case TRACK_ELEM_25_DEG_UP_TO_FLAT:
			return paint_monorail_track_25_deg_up_to_flat;

		case TRACK_ELEM_25_DEG_DOWN:
			return paint_monorail_track_25_deg_down;
		case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
			return paint_monorail_track_flat_to_25_deg_down;
		case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
			return paint_monorail_track_25_deg_down_to_flat;

		case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
			return paint_monorail_track_left_quarter_turn_5_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
			return paint_monorail_track_right_quarter_turn_5_tiles;

		case TRACK_ELEM_S_BEND_LEFT:
			return paint_monorail_track_s_bend_left;
		case TRACK_ELEM_S_BEND_RIGHT:
			return paint_monorail_track_s_bend_right;

		case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
			return paint_monorail_track_left_quarter_turn_3_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
			return paint_monorail_track_right_quarter_turn_3_tiles;

		case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
			return paint_monorail_track_left_eighth_to_diag;
		case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
			return paint_monorail_track_right_eighth_to_diag;
		case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
			return paint_monorail_track_left_eighth_to_orthogonal;
		case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
			return paint_monorail_track_right_eighth_to_orthogonal;

		case TRACK_ELEM_DIAG_FLAT:
			return paint_monorail_track_diag_flat;

		case TRACK_ELEM_DIAG_25_DEG_UP:
			return paint_monorail_track_diag_25_deg_up;
		case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
			return paint_monorail_track_diag_flat_to_25_deg_up;
		case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
			return paint_monorail_track_diag_25_deg_up_to_flat;

		case TRACK_ELEM_DIAG_25_DEG_DOWN:
			return paint_monorail_track_diag_25_deg_down;
		case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
			return paint_monorail_track_diag_flat_to_25_deg_down;
		case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
			return paint_monorail_track_diag_25_deg_down_to_flat;
	}

	return NULL;
}
