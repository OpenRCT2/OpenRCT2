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
#include "../../interface/viewport.h"
#include "../../paint/supports.h"
#include "../../paint/paint.h"
#include "../track.h"
#include "../track_paint.h"

enum {
	SPR_WATER_RC_FLAT_SW_NE = 27807,
	SPR_WATER_RC_FLAT_NW_SE = 27808,
	SPR_WATER_RC_25_DEG_SW_NE = 27809,
	SPR_WATER_RC_FLAT_TO_25_DEG_UP_SW_NE = 27811,
	SPR_WATER_RC_25_DEG_NE_SW = 27813,
	SPR_WATER_RC_FLAT_TO_25_DEG_UP_NE_SW = 27815,
	SPR_WATER_RC_25_DEG_NW_SE = 27821,
	SPR_WATER_RC_25_DEG_SE_NW = 27823,
	SPR_WATER_RC_25_DEG_UP_TO_FLAT_NE_SW = 27817,
	SPR_WATER_RC_25_DEG_UP_TO_FLAT_SW_NE = 27819,
	SPR_WATER_RC_FLAT_TO_25_DEG_UP_NW_SE = 27825,
	SPR_WATER_RC_FLAT_TO_25_DEG_UP_SE_NW = 27827,
	SPR_WATER_RC_25_DEG_UP_TO_FLAT_SE_NW = 27829,
	SPR_WATER_RC_25_DEG_UP_TO_FLAT_NW_SE = 27831,
	SPR_WATER_RC_FLAT_TO_25_DEG_UP_CHAIN_SW_NE = 27985,
	SPR_WATER_RC_FLAT_TO_25_DEG_UP_CHAIN_NW_SE = 27986,
	SPR_WATER_RC_FLAT_TO_25_DEG_UP_CHAIN_NE_SW = 27987,
	SPR_WATER_RC_FLAT_TO_25_DEG_UP_CHAIN_SE_NW = 27988,
	SPR_WATER_RC_25_DEG_CHAIN_SW_NE = 27993,
	SPR_WATER_RC_25_DEG_CHAIN_NW_SE = 27994,
	SPR_WATER_RC_25_DEG_CHAIN_NE_SW = 27995,
	SPR_WATER_RC_25_DEG_CHAIN_SE_NW = 27996,
	SPR_WATER_RC_FLAT_CHAIN_SW_NE = 27983,
	SPR_WATER_RC_FLAT_CHAIN_NW_SE = 27984,
	SPR_WATER_RC_25_DEG_UP_TO_FLAT_CHAIN_SW_NE = 27989,
	SPR_WATER_RC_25_DEG_UP_TO_FLAT_CHAIN_NW_SE = 27990,
	SPR_WATER_RC_25_DEG_UP_TO_FLAT_CHAIN_NE_SW = 27991,
	SPR_WATER_RC_25_DEG_UP_TO_FLAT_CHAIN_SE_NW = 27992,
	SPR_WATER_RC_DIAG_FLAT_W_E = 28197,
	SPR_WATER_RC_DIAG_FLAT_N_S = 28198,
	SPR_WATER_RC_DIAG_FLAT_E_W = 28199,
	SPR_WATER_RC_DIAG_FLAT_S_N = 28200,
	SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_W_E = 28201,
	SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_N_S = 28202,
	SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_E_W = 28203,
	SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_S_N = 28204,
	SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_W_E = 28205,
	SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_N_S = 28206,
	SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_E_W = 28207,
	SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_S_N = 28208,
	SPR_WATER_RC_DIAG_25_DEG_UP_W_E = 28209,
	SPR_WATER_RC_DIAG_25_DEG_UP_N_S = 28210,
	SPR_WATER_RC_DIAG_25_DEG_UP_E_W = 28211,
	SPR_WATER_RC_DIAG_25_DEG_UP_S_N = 28212,
	SPR_WATER_RC_DIAG_FLAT_CHAIN_W_E = 28269,
	SPR_WATER_RC_DIAG_FLAT_CHAIN_N_S = 28270,
	SPR_WATER_RC_DIAG_FLAT_CHAIN_E_W = 28271,
	SPR_WATER_RC_DIAG_FLAT_CHAIN_S_N = 28272,
	SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_W_E = 28273,
	SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_N_S = 28274,
	SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_E_W = 28275,
	SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_S_N = 28276,
	SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_W_E = 28277,
	SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_N_S = 28278,
	SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_E_W = 28279,
	SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_S_N = 28280,
	SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_W_E = 28281,
	SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_N_S = 28282,
	SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_E_W = 28283,
	SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_S_N = 28284,
};

static const uint32 water_rc_track_pieces_flat[2][4] = {
	{
		SPR_WATER_RC_FLAT_SW_NE,
		SPR_WATER_RC_FLAT_NW_SE,
		SPR_WATER_RC_FLAT_SW_NE,
		SPR_WATER_RC_FLAT_NW_SE,
	},
	{
		SPR_WATER_RC_FLAT_CHAIN_SW_NE,
		SPR_WATER_RC_FLAT_CHAIN_NW_SE,
		SPR_WATER_RC_FLAT_CHAIN_SW_NE,
		SPR_WATER_RC_FLAT_CHAIN_NW_SE,
	},
};

static const uint32 water_rc_track_pieces_25_deg_up[2][4] = {
	{
		SPR_WATER_RC_25_DEG_SW_NE,
		SPR_WATER_RC_25_DEG_NW_SE,
		SPR_WATER_RC_25_DEG_NE_SW,
		SPR_WATER_RC_25_DEG_SE_NW,
	},
	{
		SPR_WATER_RC_25_DEG_CHAIN_SW_NE,
		SPR_WATER_RC_25_DEG_CHAIN_NW_SE,
		SPR_WATER_RC_25_DEG_CHAIN_NE_SW,
		SPR_WATER_RC_25_DEG_CHAIN_SE_NW,
	},
};

static const uint32 water_rc_track_pieces_flat_to_25_deg_up[2][4] = {
	{
		SPR_WATER_RC_FLAT_TO_25_DEG_UP_SW_NE,
		SPR_WATER_RC_FLAT_TO_25_DEG_UP_NW_SE,
		SPR_WATER_RC_FLAT_TO_25_DEG_UP_NE_SW,
		SPR_WATER_RC_FLAT_TO_25_DEG_UP_SE_NW,
	},
	{
		SPR_WATER_RC_FLAT_TO_25_DEG_UP_CHAIN_SW_NE,
		SPR_WATER_RC_FLAT_TO_25_DEG_UP_CHAIN_NW_SE,
		SPR_WATER_RC_FLAT_TO_25_DEG_UP_CHAIN_NE_SW,
		SPR_WATER_RC_FLAT_TO_25_DEG_UP_CHAIN_SE_NW,
	},
};

static const uint32 water_rc_track_pieces_25_deg_up_to_flat[2][4] = {
	{
		SPR_WATER_RC_25_DEG_UP_TO_FLAT_SW_NE,
		SPR_WATER_RC_25_DEG_UP_TO_FLAT_NW_SE,
		SPR_WATER_RC_25_DEG_UP_TO_FLAT_NE_SW,
		SPR_WATER_RC_25_DEG_UP_TO_FLAT_SE_NW
	},
	{
		SPR_WATER_RC_25_DEG_UP_TO_FLAT_CHAIN_SW_NE,
		SPR_WATER_RC_25_DEG_UP_TO_FLAT_CHAIN_NW_SE,
		SPR_WATER_RC_25_DEG_UP_TO_FLAT_CHAIN_NE_SW,
		SPR_WATER_RC_25_DEG_UP_TO_FLAT_CHAIN_SE_NW,
	},
};

static const uint32 water_rc_track_pieces_diag_flat[2][4] = {
	{
		SPR_WATER_RC_DIAG_FLAT_W_E,
		SPR_WATER_RC_DIAG_FLAT_N_S,
		SPR_WATER_RC_DIAG_FLAT_E_W,
		SPR_WATER_RC_DIAG_FLAT_S_N
	},
	{
		SPR_WATER_RC_DIAG_FLAT_CHAIN_W_E,
		SPR_WATER_RC_DIAG_FLAT_CHAIN_N_S,
		SPR_WATER_RC_DIAG_FLAT_CHAIN_E_W,
		SPR_WATER_RC_DIAG_FLAT_CHAIN_S_N
	},
};

static const uint32 water_rc_track_pieces_diag_25_deg_up[2][4] = {
	{
		SPR_WATER_RC_DIAG_25_DEG_UP_W_E,
		SPR_WATER_RC_DIAG_25_DEG_UP_N_S,
		SPR_WATER_RC_DIAG_25_DEG_UP_E_W,
		SPR_WATER_RC_DIAG_25_DEG_UP_S_N
	},
	{
		SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_W_E,
		SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_N_S,
		SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_E_W,
		SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_S_N
	},
};

static const uint32 water_rc_track_pieces_diag_flat_to_25_deg_up[2][4] = {
	{
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_W_E,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_N_S,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_E_W,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_S_N
	},
	{
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_W_E,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_N_S,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_E_W,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_S_N
	},
};

static const uint32 water_rc_track_pieces_diag_25_deg_up_to_flat[2][4] = {
	{
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_W_E,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_N_S,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_E_W,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_S_N
	},
	{
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_W_E,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_N_S,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_E_W,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_S_N
	},
};

static const uint32 water_rc_track_pieces_diag_25_deg_down[2][4] = {
	{
		SPR_WATER_RC_DIAG_25_DEG_UP_E_W,
		SPR_WATER_RC_DIAG_25_DEG_UP_S_N,
		SPR_WATER_RC_DIAG_25_DEG_UP_W_E,
		SPR_WATER_RC_DIAG_25_DEG_UP_N_S,
	},
	{
		SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_E_W,
		SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_S_N,
		SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_W_E,
		SPR_WATER_RC_DIAG_25_DEG_UP_CHAIN_N_S,
	},
};

static const uint32 water_rc_track_pieces_diag_flat_to_25_deg_down[2][4] = {
	{
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_E_W,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_S_N,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_W_E,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_N_S,
	},
	{
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_E_W,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_S_N,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_W_E,
		SPR_WATER_RC_DIAG_25_DEG_UP_TO_FLAT_CHAIN_N_S,
	},
};

static const uint32 water_rc_track_pieces_diag_25_deg_down_to_flat[2][4] = {
	{
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_E_W,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_S_N,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_W_E,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_N_S,
	},
	{
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_E_W,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_S_N,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_W_E,
		SPR_WATER_RC_DIAG_FLAT_TO_25_DEG_UP_CHAIN_N_S,
	},
};

static const uint8 water_rc_diag_support_segment[] = {1, 0, 2, 3};

static const int water_rc_diag_blocked_segments[] = {
	SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4 | SEGMENT_BC,
	SEGMENT_C4 | SEGMENT_CC | SEGMENT_C8 | SEGMENT_B4,
	SEGMENT_D0 | SEGMENT_C4 | SEGMENT_C0 | SEGMENT_D4,
	SEGMENT_D0 | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_C8
};

static void water_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	uint32 imageId = water_rc_track_pieces_flat[isChained][direction] | gTrackColours[SCHEME_TRACK];
	sub_98196C_rotated(direction, imageId, 0, 6, 32, 20, 1, height);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);

	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup((direction & 1) ? 2 : 1, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void water_rc_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	uint32 image_id = gTrackColours[SCHEME_TRACK];
	image_id |= water_rc_track_pieces_25_deg_up[isChained][direction];

	sint8 tunnel_height[4] = { -8, 8, 8, -8 };
	uint8 tunnel_type[4] = { TUNNEL_1, TUNNEL_2, TUNNEL_2, TUNNEL_1 };
	sub_98196C_rotated(direction, image_id, 0, 6, 32, 20, 1, height);
	paint_util_push_tunnel_rotated(direction, height + tunnel_height[direction], tunnel_type[direction]);

	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		int supportType = direction & 1 ? 2 : 1;
		metal_a_supports_paint_setup(supportType, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_D0 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void water_rc_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	uint32 image_id = gTrackColours[SCHEME_TRACK];
	image_id |= water_rc_track_pieces_flat_to_25_deg_up[isChained][direction];
	sub_98196C_rotated(direction, image_id, 0, 6, 32, 20, 1, height);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
	}

	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		int supportType = direction & 1 ? 2 : 1;
		uint16 ax = (direction == 0) ? 5 : 3;
		metal_a_supports_paint_setup(supportType, 4, ax, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_D0 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void water_rc_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	uint32 image_id = gTrackColours[SCHEME_TRACK];
	image_id |= water_rc_track_pieces_25_deg_up_to_flat[isChained][direction];

	uint8 tunnelType;
	sint16 tunnelHeight;
	if (direction == 1 || direction == 2) {
		tunnelType = TUNNEL_12;
		tunnelHeight = height + 8;
	} else {
		tunnelType = TUNNEL_0;
		tunnelHeight = height - 8;
	}
	sub_98196C_rotated(direction, image_id, 0, 6, 32, 20, 1, height);
	paint_util_push_tunnel_rotated(direction, tunnelHeight, tunnelType);

	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		int supportType = direction & 1 ? 2 : 1;
		metal_a_supports_paint_setup(supportType, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_D0 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void water_rc_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement)
{
	water_rc_track_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void water_rc_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement)
{
	water_rc_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void water_rc_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement)
{
	water_rc_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void water_rc_track_diag_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	track_paint_util_diag_tiles_paint(
		1,
		height,
		direction,
		trackSequence,
		gTrackColours[SCHEME_TRACK],
		water_rc_track_pieces_diag_flat[isChained],
		defaultDiagTileOffsets,
		defaultDiagBoundLengths,
		0,
		get_current_rotation());

	if (trackSequence == 3) {
		metal_a_supports_paint_setup(direction & 1 ? 2 : 1, water_rc_diag_support_segment[direction], 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = water_rc_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void water_rc_track_diag_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	track_paint_util_diag_tiles_paint(
		1,
		height,
		direction,
		trackSequence,
		gTrackColours[SCHEME_TRACK],
		water_rc_track_pieces_diag_25_deg_up[isChained],
		defaultDiagTileOffsets,
		defaultDiagBoundLengths,
		0,
		get_current_rotation());

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(direction & 1 ? 2 : 1, water_rc_diag_support_segment[direction], 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = water_rc_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void water_rc_track_diag_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	track_paint_util_diag_tiles_paint(
		1,
		height,
		direction,
		trackSequence,
		gTrackColours[SCHEME_TRACK],
		water_rc_track_pieces_diag_flat_to_25_deg_up[isChained],
		defaultDiagTileOffsets,
		defaultDiagBoundLengths,
		0,
		get_current_rotation());

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(direction & 1 ? 2 : 1, water_rc_diag_support_segment[direction], 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = water_rc_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void water_rc_track_diag_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	track_paint_util_diag_tiles_paint(
		1,
		height,
		direction,
		trackSequence,
		gTrackColours[SCHEME_TRACK],
		water_rc_track_pieces_diag_25_deg_up_to_flat[isChained],
		defaultDiagTileOffsets,
		defaultDiagBoundLengths,
		0,
		get_current_rotation());

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(direction & 1 ? 2 : 1, water_rc_diag_support_segment[direction], 4, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = water_rc_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void water_rc_track_diag_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	track_paint_util_diag_tiles_paint(
		1,
		height,
		direction,
		trackSequence,
		gTrackColours[SCHEME_TRACK],
		water_rc_track_pieces_diag_25_deg_down[isChained],
		defaultDiagTileOffsets,
		defaultDiagBoundLengths,
		0,
		get_current_rotation());

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(direction & 1 ? 2 : 1, water_rc_diag_support_segment[direction], 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = water_rc_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void water_rc_track_diag_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	track_paint_util_diag_tiles_paint(
		1,
		height,
		direction,
		trackSequence,
		gTrackColours[SCHEME_TRACK],
		water_rc_track_pieces_diag_flat_to_25_deg_down[isChained],
		defaultDiagTileOffsets,
		defaultDiagBoundLengths,
		0,
		get_current_rotation());

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(direction & 1 ? 2 : 1, water_rc_diag_support_segment[direction], 4, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = water_rc_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void water_rc_track_diag_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	track_paint_util_diag_tiles_paint(
		1,
		height,
		direction,
		trackSequence,
		gTrackColours[SCHEME_TRACK],
		water_rc_track_pieces_diag_25_deg_down_to_flat[isChained],
		defaultDiagTileOffsets,
		defaultDiagBoundLengths,
		0,
		get_current_rotation());

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(direction & 1 ? 2 : 1, water_rc_diag_support_segment[direction], 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = water_rc_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_water_rc(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT:
		return water_rc_track_flat;
	case TRACK_ELEM_25_DEG_UP:
		return water_rc_track_25_deg_up;
	case TRACK_ELEM_FLAT_TO_25_DEG_UP:
		return water_rc_track_flat_to_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_FLAT:
		return water_rc_track_25_deg_up_to_flat;
	case TRACK_ELEM_25_DEG_DOWN:
		return water_rc_track_25_deg_down;
	case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
		return water_rc_track_flat_to_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
		return water_rc_track_25_deg_down_to_flat;
	case TRACK_ELEM_DIAG_FLAT:
		return water_rc_track_diag_flat;
	case TRACK_ELEM_DIAG_25_DEG_UP:
		return water_rc_track_diag_25_deg_up;
	case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
		return water_rc_track_diag_flat_to_25_deg_up;
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
		return water_rc_track_diag_25_deg_up_to_flat;
	case TRACK_ELEM_DIAG_25_DEG_DOWN:
		return water_rc_track_diag_25_deg_down;
	case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
		return water_rc_track_diag_flat_to_25_deg_down;
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
		return water_rc_track_diag_25_deg_down_to_flat;

	//////////////////////////////////////////////////////
	// Use Junior RC
	//////////////////////////////////////////////////////
	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
	case TRACK_ELEM_60_DEG_UP:
	case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
	case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
	case TRACK_ELEM_60_DEG_DOWN:
	case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
	case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
	case TRACK_ELEM_FLAT_TO_LEFT_BANK:
	case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
	case TRACK_ELEM_LEFT_BANK_TO_FLAT:
	case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
	case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
	case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
	case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
	case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
	case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
	case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
	case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
	case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
	case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
	case TRACK_ELEM_LEFT_BANK:
	case TRACK_ELEM_RIGHT_BANK:
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
	case TRACK_ELEM_S_BEND_LEFT:
	case TRACK_ELEM_S_BEND_RIGHT:
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
	case TRACK_ELEM_FLAT_TO_60_DEG_UP:
	case TRACK_ELEM_60_DEG_UP_TO_FLAT:
	case TRACK_ELEM_FLAT_TO_60_DEG_DOWN:
	case TRACK_ELEM_60_DEG_DOWN_TO_FLAT:
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
	case TRACK_ELEM_BRAKES:
	case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
	case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
	case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
	case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
	case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
	case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
	case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
	case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
	case TRACK_ELEM_DIAG_60_DEG_UP:
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
	case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
	case TRACK_ELEM_DIAG_60_DEG_DOWN:
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
	case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
	case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
	case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
	case TRACK_ELEM_DIAG_LEFT_BANK:
	case TRACK_ELEM_DIAG_RIGHT_BANK:
	case TRACK_ELEM_BLOCK_BRAKES:
		return get_track_paint_function_junior_rc(trackType, direction);

	//////////////////////////////////////////////////////
	// Use River Rafts
	//////////////////////////////////////////////////////
	case TRACK_ELEM_FLAT_COVERED:
		return get_track_paint_function_river_rafts(TRACK_ELEM_FLAT, direction);
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED:
		return get_track_paint_function_river_rafts(TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES, direction);
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED:
		return get_track_paint_function_river_rafts(TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES, direction);
	}
	return NULL;
}
