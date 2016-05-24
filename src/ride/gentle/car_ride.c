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

enum {
	SPR_CAR_RIDE_FLAT_SW_NE = 28773,
	SPR_CAR_RIDE_FLAT_NW_SE = 28774,

	SPR_CAR_RIDE_25_DEG_UP_SW_NE = 28777,
	SPR_CAR_RIDE_25_DEG_UP_NE_SW = 28779,
	SPR_CAR_RIDE_25_DEG_UP_NW_SE = 28783,
	SPR_CAR_RIDE_25_DEG_UP_SE_NW = 28784,
};

static const uint32 car_ride_track_pieces_flat[4] = {
	SPR_CAR_RIDE_FLAT_SW_NE,
	SPR_CAR_RIDE_FLAT_NW_SE,
	SPR_CAR_RIDE_FLAT_SW_NE,
	SPR_CAR_RIDE_FLAT_NW_SE,
};

static const uint32 car_ride_track_pieces_25_deg_up[4] = {
	SPR_CAR_RIDE_25_DEG_UP_SW_NE,
	SPR_CAR_RIDE_25_DEG_UP_NW_SE,
	SPR_CAR_RIDE_25_DEG_UP_NE_SW,
	SPR_CAR_RIDE_25_DEG_UP_SE_NW,
};


/** rct2: 0x006F72C8 */
static void paint_car_ride_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId = car_ride_track_pieces_flat[direction] | RCT2_GLOBAL(0x00F44198, uint32);

	if (direction == 0 || direction == 2) {
		sub_98196C(imageId, 0, 6, 32, 20, 1, height, get_current_rotation());
	} else {
		sub_98196C(imageId, 6, 0, 20, 32, 1, height, get_current_rotation());
	}

	if (direction == 0 || direction == 2) {
		paint_util_push_tunnel_left(height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_right(height, TUNNEL_0);
	}

	metal_a_supports_paint_setup(3, 4, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x006F72D8 */
static void paint_car_ride_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId = car_ride_track_pieces_25_deg_up[direction] | RCT2_GLOBAL(0x00F44198, uint32);

	if (direction == 0 || direction == 2) {
		sub_98197C(imageId, 0, 2, 32, 20, 1, height, 0, 6, height, get_current_rotation());
	} else {
		sub_98197C(imageId, 2, 0, 20, 32, 1, height, 6, 0, height, get_current_rotation());
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

	metal_a_supports_paint_setup(3, 4, 8, height, RCT2_GLOBAL(0x00F4419C, uint32));
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x */
static void paint_car_ride_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x006F7308 */
static void paint_car_ride_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	paint_car_ride_track_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x */
static void paint_car_ride_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_right_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_left_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_left_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_right_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_spinning_tunnel(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_car_ride_track_log_bumps(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/**
 * rct2: 0x006F7000
 */
TRACK_PAINT_FUNCTION get_track_paint_function_car_ride(int trackType, int direction)
{
	switch (trackType) {
		case TRACK_ELEM_FLAT:
			return paint_car_ride_track_flat;

		case TRACK_ELEM_END_STATION:
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
			return paint_car_ride_station;

		case TRACK_ELEM_25_DEG_UP:
			return paint_car_ride_track_25_deg_up;
		case TRACK_ELEM_60_DEG_UP:
			return paint_car_ride_track_60_deg_up;
		case TRACK_ELEM_FLAT_TO_25_DEG_UP:
			return paint_car_ride_track_flat_to_25_deg_up;
		case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
			return paint_car_ride_track_25_deg_up_to_60_deg_up;
		case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
			return paint_car_ride_track_60_deg_up_to_25_deg_up;
		case TRACK_ELEM_25_DEG_UP_TO_FLAT:
			return paint_car_ride_track_25_deg_up_to_flat;

		case TRACK_ELEM_25_DEG_DOWN:
			return paint_car_ride_track_25_deg_down;
		case TRACK_ELEM_60_DEG_DOWN:
			return paint_car_ride_track_60_deg_down;
		case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
			return paint_car_ride_track_flat_to_25_deg_down;
		case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
			return paint_car_ride_track_25_deg_down_to_60_deg_down;
		case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
			return paint_car_ride_track_60_deg_down_to_25_deg_down;
		case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
			return paint_car_ride_track_25_deg_down_to_flat;

		case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
			return paint_car_ride_track_left_quarter_turn_3_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
			return paint_car_ride_track_right_quarter_turn_3_tiles;

		case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
			return paint_car_ride_track_left_quarter_turn_1_tile;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
			return paint_car_ride_track_right_quarter_turn_1_tile;

		case TRACK_ELEM_RAPIDS:
			return paint_car_ride_track_log_bumps;

		case TRACK_ELEM_SPINNING_TUNNEL:
			return paint_car_ride_track_spinning_tunnel;
	}

	return NULL;
}