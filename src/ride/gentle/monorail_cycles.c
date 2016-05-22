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
#include "../../world/map.h"

enum
{
	SPR_MONORAIL_CYCLES_FLAT_SW_NE = 16820,
	SPR_MONORAIL_CYCLES_FLAT_NW_SE = 16821,
};

static const uint32 monorail_cycles_track_pieces_flat[4] = {
	SPR_MONORAIL_CYCLES_FLAT_SW_NE,
	SPR_MONORAIL_CYCLES_FLAT_NW_SE
};

static paint_struct * paint_monorail_cycles_util_7c(
	bool flip,
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	sint16 z_offset,
	sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
	uint32 rotation
)
{
	if (flip) {
		return sub_98197C(image_id, y_offset, x_offset, bound_box_length_y, bound_box_length_x, bound_box_length_z, z_offset, bound_box_offset_y, bound_box_offset_x, bound_box_offset_z, rotation);
	}

	return sub_98197C(image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset, bound_box_offset_x, bound_box_offset_y, bound_box_offset_z, rotation);
}

/** rct2: 0x0088AD48 */
static void paint_monorail_cycles_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId = monorail_cycles_track_pieces_flat[(direction & 1)] | RCT2_GLOBAL(0x00F44198, uint32);
	paint_monorail_cycles_util_7c((bool) (direction & 1), imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());

	if (direction & 1) {
		paint_util_push_tunnel_right(height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_left(height, TUNNEL_0);
	}

	metal_a_supports_paint_setup((direction & 1) ? 5 : 4, 4, -1, height, RCT2_GLOBAL(0x00F4419C, uint32));

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x*/
static void paint_monorail_cycles_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_left_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_right_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_left_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_right_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/**
 * rct2: 0x0088ac88
 */
TRACK_PAINT_FUNCTION get_track_paint_function_monorail_cycles(int trackType, int direction) {
	switch(trackType) {
		case TRACK_ELEM_FLAT:
			return paint_monorail_cycles_track_flat;

		case TRACK_ELEM_END_STATION:
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
			return paint_monorail_cycles_station;

		case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
			return paint_monorail_cycles_track_left_quarter_turn_5_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
			return paint_monorail_cycles_track_right_quarter_turn_5_tiles;

		case TRACK_ELEM_S_BEND_LEFT:
			return paint_monorail_cycles_track_s_bend_left;
		case TRACK_ELEM_S_BEND_RIGHT:
			return paint_monorail_cycles_track_s_bend_right;

		case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
			return paint_monorail_cycles_track_left_quarter_turn_3_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
			return paint_monorail_cycles_track_right_quarter_turn_3_tiles;
	}

	return NULL;
}
