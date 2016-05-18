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

/**
 *
 *  rct2: 0x006D44D5
 */
void vehicle_visual_submarine(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	int baseImage_id = imageDirection;
	int image_id;
	if (vehicle->restraints_position >= 64) {
		if ((vehicleEntry->sprite_flags & 0x2000) && !(imageDirection & 3)) {
			baseImage_id /= 8;
			baseImage_id += ((vehicle->restraints_position - 64) / 64) * 4;
			baseImage_id *= vehicleEntry->var_16;
			baseImage_id += vehicleEntry->var_1C;
		}
	} else {
		if (vehicleEntry->flags_a & 0x800) {
			baseImage_id /= 2;
		}
		if (vehicleEntry->sprite_flags & 0x8000) {
			baseImage_id /= 8;
		}
		baseImage_id *= vehicleEntry->var_16;
		baseImage_id += vehicleEntry->base_image_id;
		baseImage_id += vehicle->var_4A;
	}

	vehicle_boundbox bb = VehicleBoundboxes[vehicleEntry->draw_order][imageDirection / 2];

	image_id = baseImage_id | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0x80000000;
	paint_struct* ps = sub_98197C(image_id, 0, 0, bb.length_x, bb.length_y, bb.length_z, z, bb.offset_x, bb.offset_y, bb.offset_z + z, get_current_rotation());
	if (ps != NULL) {
		ps->tertiary_colour = vehicle->colours_extended;
	}

	image_id = (baseImage_id + 1) | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0x80000000;
	ps = sub_98197C(image_id, 0, 0, bb.length_x, bb.length_y, 2, z, bb.offset_x, bb.offset_y, bb.offset_z + z - 10, get_current_rotation());
	if (ps != NULL) {
		ps->tertiary_colour = vehicle->colours_extended;
	}

	assert(vehicleEntry->effect_visual == 1);
}

enum
{
	SPR_SUBMARINE_RIDE_FLAT_NE_SW = 16870,
	SPR_SUBMARINE_RIDE_FLAT_SE_NW = 16871,

	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 16872,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 16873,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 16874,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 16875,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 16876,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 16877,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 16878,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 16879,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 16880,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 16881,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 16882,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 16883,

	SPR_SUBMARINE_RIDE_FLAT_TO_25_DEG_UP_SW_NE,
	SPR_SUBMARINE_RIDE_FLAT_TO_25_DEG_UP_NW_SE,
	SPR_SUBMARINE_RIDE_FLAT_TO_25_DEG_UP_NE_SW,
	SPR_SUBMARINE_RIDE_FLAT_TO_25_DEG_UP_SE_NW,

	SPR_SUBMARINE_RIDE_25_DEG_UP_TO_FLAT_SW_NE,
	SPR_SUBMARINE_RIDE_25_DEG_UP_TO_FLAT_NW_SE,
	SPR_SUBMARINE_RIDE_25_DEG_UP_TO_FLAT_NE_SW,
	SPR_SUBMARINE_RIDE_25_DEG_UP_TO_FLAT_SE_NW,

	SPR_SUBMARINE_RIDE_25_DEG_UP_SW_NE,
	SPR_SUBMARINE_RIDE_25_DEG_UP_NW_SE,
	SPR_SUBMARINE_RIDE_25_DEG_UP_NE_SW,
	SPR_SUBMARINE_RIDE_25_DEG_UP_SE_NW,

	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_1_TILE_SW_NW = 16896,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_1_TILE_NW_NE = 16897,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_1_TILE_NE_SE = 16898,
	SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_1_TILE_SE_SW = 16899,
};

static void submarine_ride_paint_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_xy16 position = {RCT2_GLOBAL(0x009DE56A, sint16), RCT2_GLOBAL(0x009DE56E, sint16)};
	rct_ride * ride = get_ride(rideIndex);
	const rct_ride_entrance_definition * entranceStyle = &RideEntranceDefinitions[ride->entrance_style];
	int heightLower = height - 16;
	uint32 imageId;
	bool hasFence;

	if (direction & 1) {
		imageId = SPR_SUBMARINE_RIDE_FLAT_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 20, 32, 3, heightLower, 6, 0, heightLower, get_current_rotation());
		paint_util_push_tunnel_right(height, TUNNEL_6);

		hasFence = track_paint_util_has_fence(EDGE_NE, position, mapElement, ride, get_current_rotation());
		imageId = (hasFence ? SPR_STATION_PIER_EDGE_NE_FENCED : SPR_STATION_PIER_EDGE_NE) | RCT2_GLOBAL(0x00F4419C, uint32);
		sub_98197C(imageId, 0, 0, 6, 32, 1, height, 2, 0, height, get_current_rotation());
		track_paint_util_draw_station_covers(EDGE_NE, hasFence, entranceStyle, direction, height);

		imageId = SPR_STATION_PIER_EDGE_SW | RCT2_GLOBAL(0x00F4419C, uint32);
		sub_98196C(imageId, 24, 0, 8, 32, 1, height, get_current_rotation());

		hasFence = track_paint_util_has_fence(EDGE_SW, position, mapElement, ride, get_current_rotation());
		if (hasFence) {
			imageId = SPR_STATION_PIER_FENCE_SW | RCT2_GLOBAL(0x00F4419C, uint32);
			sub_98196C(imageId, 31, 0, 1, 32, 7, height + 2, get_current_rotation());
		}
		track_paint_util_draw_station_covers(EDGE_SW, hasFence, entranceStyle, direction, height);

	} else {
		imageId = SPR_SUBMARINE_RIDE_FLAT_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 20, 3, heightLower, 0, 6, heightLower, get_current_rotation());
		paint_util_push_tunnel_left(height, TUNNEL_6);

		hasFence = track_paint_util_has_fence(EDGE_NW, position, mapElement, ride, get_current_rotation());
		imageId = (hasFence ? SPR_STATION_PIER_EDGE_NW_FENCED : SPR_STATION_PIER_EDGE_NW) | RCT2_GLOBAL(0x00F4419C, uint32);
		sub_98197C(imageId, 0, 0, 32, 6, 1, height, 0, 2, height, get_current_rotation());
		track_paint_util_draw_station_covers(EDGE_NW, hasFence, entranceStyle, direction, height);

		imageId = SPR_STATION_PIER_EDGE_SE | RCT2_GLOBAL(0x00F4419C, uint32);
		sub_98196C(imageId, 0, 24, 32, 8, 1, height, get_current_rotation());

		hasFence = track_paint_util_has_fence(EDGE_SE, position, mapElement, ride, get_current_rotation());
		if (hasFence) {
			imageId = SPR_STATION_PIER_FENCE_SE | RCT2_GLOBAL(0x00F4419C, uint32);
			sub_98196C(imageId, 0, 31, 32, 1, 7, height + 2, get_current_rotation());
		}
		track_paint_util_draw_station_covers(EDGE_SE, hasFence, entranceStyle, direction, height);
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void submarine_ride_paint_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_xy16 position = {RCT2_GLOBAL(0x009DE56A, sint16), RCT2_GLOBAL(0x009DE56E, sint16)};
	int heightLower = height - 16;
	uint32 imageId;

	if (direction & 1) {
		imageId = SPR_SUBMARINE_RIDE_FLAT_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 20, 32, 3, heightLower, 6, 0, heightLower, get_current_rotation());
		paint_util_push_tunnel_right(heightLower, TUNNEL_0);
	} else {
		imageId = SPR_SUBMARINE_RIDE_FLAT_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 20, 3, heightLower, 0, 6, heightLower, get_current_rotation());
		paint_util_push_tunnel_left(heightLower, TUNNEL_0);
	}

	if (track_paint_util_should_paint_supports(position)) {
		metal_a_supports_paint_setup((direction & 1) ? 4 : 5, 4, -1, heightLower, RCT2_GLOBAL(0x00F4419C, uint32));
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 16, 0x20);
}

static void submarine_ride_paint_track_quarter_turn_3_tiles_nw_sw(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	int heightLower = height - 16;
	uint32 imageId;

	switch (trackSequence) {
		case 0:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 32, 20, 3, heightLower, 0, 6, heightLower, get_current_rotation());

			paint_util_push_tunnel_right(heightLower, TUNNEL_0);
			metal_a_supports_paint_setup(4, 4, -1, heightLower, RCT2_GLOBAL(0x00F4419C, uint32));
			paint_util_set_segment_support_height(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4, 0xFFFF, 0);
			break;
		case 1:
			break;
		case 2:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 16, 16, 3, heightLower, 16, 0, heightLower, get_current_rotation());

			paint_util_set_segment_support_height(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8, 0xFFFF, 0);
			break;
		case 3:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 20, 32, 3, heightLower, 6, 0, heightLower, get_current_rotation());

			metal_a_supports_paint_setup(4, 4, -1, heightLower, RCT2_GLOBAL(0x00F4419C, uint32));
			paint_util_set_segment_support_height(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0, 0xFFFF, 0);
			break;
	}

	paint_util_set_general_support_height(height + 16, 0x20);
}

static void submarine_ride_paint_track_quarter_turn_3_tiles_ne_nw(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	int heightLower = height - 16;
	uint32 imageId;

	switch (trackSequence) {
		case 0:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 20, 32, 3, heightLower, 6, 0, heightLower, get_current_rotation());

			metal_a_supports_paint_setup(4, 4, -1, heightLower, RCT2_GLOBAL(0x00F4419C, uint32));
			paint_util_set_segment_support_height(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC, 0xFFFF, 0);
			break;
		case 1:
			break;
		case 2:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 16, 16, 3, heightLower, 0, 0, heightLower, get_current_rotation());

			paint_util_set_segment_support_height(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C8, 0xFFFF, 0);
			break;
		case 3:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 32, 20, 3, heightLower, 0, 6, heightLower, get_current_rotation());

			metal_a_supports_paint_setup(4, 4, -1, heightLower, RCT2_GLOBAL(0x00F4419C, uint32));
			paint_util_set_segment_support_height(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8, 0xFFFF, 0);
			break;
	}

	paint_util_set_general_support_height(height + 16, 0x20);
}

static void submarine_ride_paint_track_quarter_turn_3_tiles_se_ne(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	int heightLower = height - 16;
	uint32 imageId;

	switch (trackSequence) {
		case 0:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 32, 20, 3, heightLower, 0, 6, heightLower, get_current_rotation());

			metal_a_supports_paint_setup(4, 4, -1, heightLower, RCT2_GLOBAL(0x00F4419C, uint32));
			paint_util_set_segment_support_height(SEGMENT_CC | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_C0, 0xFFFF, 0);
			break;
		case 1:
			break;
		case 2:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 16, 16, 3, heightLower, 0, 16, heightLower, get_current_rotation());

			paint_util_set_segment_support_height(SEGMENT_CC | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC, 0xFFFF, 0);
			break;
		case 3:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 20, 32, 3, heightLower, 6, 0, heightLower, get_current_rotation());

			metal_a_supports_paint_setup(4, 4, -1, heightLower, RCT2_GLOBAL(0x00F4419C, uint32));
			paint_util_set_segment_support_height(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC, 0xFFFF, 0);
			paint_util_push_tunnel_left(heightLower, TUNNEL_0);
			break;
	}

	paint_util_set_general_support_height(height + 16, 0x20);
}

static void submarine_ride_paint_track_quarter_turn_3_tiles_sw_se(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	int heightLower = height - 16;
	uint32 imageId;

	switch (trackSequence) {
		case 0:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 20, 32, 3, heightLower, 6, 0, heightLower, get_current_rotation());

			metal_a_supports_paint_setup(4, 4, -1, heightLower, RCT2_GLOBAL(0x00F4419C, uint32));
			paint_util_set_segment_support_height(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8, 0xFFFF, 0);
			paint_util_push_tunnel_right(heightLower, TUNNEL_0);
			break;
		case 1:
			break;
		case 2:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 16, 16, 3, heightLower, 16, 16, heightLower, get_current_rotation());

			paint_util_set_segment_support_height(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0, 0xFFFF, 0);
			break;
		case 3:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0 | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 32, 20, 3, heightLower, 0, 6, heightLower, get_current_rotation());

			metal_a_supports_paint_setup(4, 4, -1, heightLower, RCT2_GLOBAL(0x00F4419C, uint32));
			paint_util_set_segment_support_height(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_B4, 0xFFFF, 0);
			break;
	}

	paint_util_set_general_support_height(height + 16, 0x20);
}

static void submarine_ride_paint_track_left_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
		case 0:
			submarine_ride_paint_track_quarter_turn_3_tiles_nw_sw(rideIndex, trackSequence, direction, height, mapElement);
			break;
		case 1:
			submarine_ride_paint_track_quarter_turn_3_tiles_ne_nw(rideIndex, trackSequence, direction, height, mapElement);
			break;
		case 2:
			submarine_ride_paint_track_quarter_turn_3_tiles_se_ne(rideIndex, trackSequence, direction, height, mapElement);
			break;
		case 3:
			submarine_ride_paint_track_quarter_turn_3_tiles_sw_se(rideIndex, trackSequence, direction, height, mapElement);
			break;
	}
}

uint8 right_quarter_turn_3_tiles_to_left_turn_map[] = {3, 1, 2, 0};
static void submarine_ride_paint_track_right_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
	submarine_ride_paint_track_left_quarter_turn_3_tiles(rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

static void submarine_ride_paint_track_left_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	int heightLower = height - 16;
	uint32 imageId;

	switch (direction) {
		case 0:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_1_TILE_SW_NW | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 26, 24, 1, heightLower, 6, 2, heightLower, get_current_rotation());
			paint_util_push_tunnel_left(heightLower, TUNNEL_0);
			break;
		case 1:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_1_TILE_NW_NE | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 26, 26, 1, heightLower, 0, 0, heightLower, get_current_rotation());
			break;
		case 2:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_1_TILE_NE_SE | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 24, 26, 1, heightLower, 2, 6, heightLower, get_current_rotation());
			paint_util_push_tunnel_right(heightLower, TUNNEL_0);
			break;
		case 3:
			imageId = SPR_SUBMARINE_RIDE_FLAT_QUARTER_TURN_1_TILE_SE_SW | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 24, 24, 1, heightLower, 6, 6, heightLower, get_current_rotation());
			paint_util_push_tunnel_right(heightLower, TUNNEL_0);
			paint_util_push_tunnel_left(heightLower, TUNNEL_0);
			break;
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 16, 0x20);
}

static void submarine_ride_paint_track_right_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	submarine_ride_paint_track_left_quarter_turn_1_tile(rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/**
 * rct2: 0x008995D4
 */
TRACK_PAINT_FUNCTION get_track_paint_function_submarine_ride(int trackType, int direction)
{
	switch (trackType) {
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
		case TRACK_ELEM_END_STATION:
			return submarine_ride_paint_track_station;

		case TRACK_ELEM_FLAT:
			return submarine_ride_paint_track_flat;

		case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
			return submarine_ride_paint_track_left_quarter_turn_3_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
			return submarine_ride_paint_track_right_quarter_turn_3_tiles;

		case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
			return submarine_ride_paint_track_left_quarter_turn_1_tile;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
			return submarine_ride_paint_track_right_quarter_turn_1_tile;
	}

	return NULL;
}
