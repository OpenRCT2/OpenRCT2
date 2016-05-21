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

// 0x009927E6:
static const vehicle_boundbox _virginiaReelBoundbox[] = {
	{ -11, -11,  1, 22, 22, 13 },
	{ -11, -11,  1, 22, 22, 13 },
	{ -11, -11,  1, 22, 22, 13 },
	{ -11, -11,  1, 22, 22, 13 },
	{ -11, -11,  1, 22, 22, 13 },
	{ -11, -11,  1, 22, 22, 13 },
	{ -11, -11,  1, 22, 22, 13 },
	{ -11, -11,  1, 22, 22, 13 },
	{ -11, -11,  1, 22, 22, 13 },
};

enum
{
	SPR_VIRGINIA_REEL_FLAT_SW_NE = 21458,
	SPR_VIRGINIA_REEL_FLAT_NW_SE = 21459,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SW_NE = 21460,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NW_SE = 21461,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NE_SW = 21462,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SE_NW = 21463,

	SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SW_NE = 21488,
	SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NW_SE = 21489,
	SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NE_SW = 21490,
	SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SE_NW = 21491,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SW_NE = 21492,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NW_SE = 21493,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NE_SW = 21494,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SE_NW = 21495,
};

static const uint32 virginia_reel_track_pieces_flat[4] = {
	SPR_VIRGINIA_REEL_FLAT_SW_NE,
	SPR_VIRGINIA_REEL_FLAT_NW_SE,
	SPR_VIRGINIA_REEL_FLAT_SW_NE,
	SPR_VIRGINIA_REEL_FLAT_NW_SE,
};

static const uint32 virginia_reel_track_pieces_flat_lift_hill[4] = {
	SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SW_NE,
	SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NW_SE,
	SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NE_SW,
	SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SE_NW,
};

static const uint32 virginia_reel_track_pieces_flat_to_25_deg_up[4] = {
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SW_NE,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NW_SE,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NE_SW,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SE_NW,
};

static const uint32 virginia_reel_track_pieces_flat_to_25_deg_up_lift_hill[4] = {
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SW_NE,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NW_SE,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NE_SW,
	SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SE_NW,
};

/**
 *
 *  rct2: 0x006D5B48
 */
void vehicle_visual_virginia_reel(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	int image_id;
	int baseImage_id = imageDirection;
	const uint8 rotation = get_current_rotation();
	int ecx = ((vehicle->var_BA / 8) + (rotation * 8)) & 31;
	int j = 0;
	if (vehicle->vehicle_sprite_type == 0) {
		baseImage_id = ecx & 7;
	} else {
		if (vehicle->vehicle_sprite_type == 1 || vehicle->vehicle_sprite_type == 5) {
			if (vehicle->vehicle_sprite_type == 5){
				baseImage_id = imageDirection ^ 16;
			}
			baseImage_id &= 24;
			j = (baseImage_id / 8) + 1;
			baseImage_id += (ecx & 7);
			baseImage_id += 8;
		} else
		if (vehicle->vehicle_sprite_type == 2 || vehicle->vehicle_sprite_type == 6) {
			if (vehicle->vehicle_sprite_type == 6){
				baseImage_id = imageDirection ^ 16;
			}
			baseImage_id &= 24;
			j = (baseImage_id / 8) + 5;
			baseImage_id += (ecx & 7);
			baseImage_id += 40;
		} else {
			baseImage_id = ecx & 7;
		}
	}
	baseImage_id += vehicleEntry->base_image_id;

	const vehicle_boundbox *bb = &_virginiaReelBoundbox[j];
	image_id = baseImage_id | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0xA0000000;
	sub_98197C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);

	if (RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*)->zoom_level < 2 && vehicle->num_peeps > 0) {
		uint8 riding_peep_sprites[4] = {0xFF, 0xFF, 0xFF, 0xFF};
		for (int i = 0; i < vehicle->num_peeps; i++) {
			riding_peep_sprites[((ecx / 8) + i) & 3] = vehicle->peep_tshirt_colours[i];
		}
		int draw_order[4] = {0, 1, 3, 2};
		for (int i = 0; i < countof(draw_order); i++) {
			if (riding_peep_sprites[draw_order[i]] != 0xFF) {
				image_id = (baseImage_id + ((draw_order[i] + 1) * 72)) | (riding_peep_sprites[draw_order[i]] << 19) | 0x20000000;
				sub_98199C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
			}
		}
	}

	assert(vehicleEntry->effect_visual == 1);
}

/** rct2: 0x00811264 */
static void paint_viriginia_reel_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	const uint32 * sprites = virginia_reel_track_pieces_flat;
	if (mapElement->type & 0x80) {
		sprites = virginia_reel_track_pieces_flat_lift_hill;
	}

	uint32 imageId = sprites[direction] | RCT2_GLOBAL(0x00F44198, uint32);
	if (direction & 1) {
		sub_98197C(imageId, 0, 0, 27, 32, 2, height, 2, 0, height, get_current_rotation());
		paint_util_push_tunnel_right(height, TUNNEL_0);
	} else {
		sub_98197C(imageId, 0, 0, 32, 27, 2, height, 0, 2, height, get_current_rotation());
		paint_util_push_tunnel_left(height, TUNNEL_0);
	}

	wooden_a_supports_paint_setup((direction & 1), 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x */
static void paint_viriginia_reel_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x00811294 */
static void paint_viriginia_reel_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	const uint32 * sprites = virginia_reel_track_pieces_flat_to_25_deg_up;
	if (mapElement->type & 0x80) {
		sprites = virginia_reel_track_pieces_flat_to_25_deg_up_lift_hill;
	}

	uint32 imageId = sprites[direction] | RCT2_GLOBAL(0x00F44198, uint32);
	paint_struct * ps;
	switch (direction) {
		case 0:
			sub_98197C(imageId, 0, 0, 32, 27, 2, height, 0, 2, height, get_current_rotation());

			wooden_a_supports_paint_setup(0, 1, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		case 1:
			ps = sub_98197C(imageId, 0, 0, 27, 32, 2, height, 2, 0, height, get_current_rotation());
			RCT2_GLOBAL(0x009DEA58, paint_struct*) = ps;

			wooden_a_supports_paint_setup(1, 2, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height, TUNNEL_8);
			break;
		case 2:
			ps = sub_98197C(imageId, 0, 0, 32, 27, 2, height, 0, 2, height, get_current_rotation());
			RCT2_GLOBAL(0x009DEA58, paint_struct*) = ps;

			wooden_a_supports_paint_setup(0, 3, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height, TUNNEL_8);
			break;
		case 3:
			sub_98197C(imageId, 0, 0, 27, 32, 2, height, 2, 0, height, get_current_rotation());

			wooden_a_supports_paint_setup(1, 4, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
	}

	wooden_a_supports_paint_setup((direction & 1), 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x */
static void paint_viriginia_reel_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_viriginia_reel_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_viriginia_reel_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x008112C4 */
static void paint_viriginia_reel_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	paint_viriginia_reel_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x */
static void paint_viriginia_reel_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_viriginia_reel_track_left_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_viriginia_reel_track_right_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_viriginia_reel_track_left_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x */
static void paint_viriginia_reel_track_right_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/**
 * rct2: 0x00811184
 */
TRACK_PAINT_FUNCTION get_track_paint_function_virginia_reel(int trackType, int direction)
{
	switch (trackType) {
		case TRACK_ELEM_FLAT:
			return paint_viriginia_reel_track_flat;

		case TRACK_ELEM_END_STATION:
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
			return paint_viriginia_reel_station;

		case TRACK_ELEM_25_DEG_UP:
			return paint_viriginia_reel_track_25_deg_up;
		case TRACK_ELEM_FLAT_TO_25_DEG_UP:
			return paint_viriginia_reel_track_flat_to_25_deg_up;
		case TRACK_ELEM_25_DEG_UP_TO_FLAT:
			return paint_viriginia_reel_track_25_deg_up_to_flat;

		case TRACK_ELEM_25_DEG_DOWN:
			return paint_viriginia_reel_track_25_deg_down;
		case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
			return paint_viriginia_reel_track_flat_to_25_deg_down;
		case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
			return paint_viriginia_reel_track_25_deg_down_to_flat;

		case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
			return paint_viriginia_reel_track_left_quarter_turn_3_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
			return paint_viriginia_reel_track_right_quarter_turn_3_tiles;

		case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
			return paint_viriginia_reel_track_left_quarter_turn_1_tile;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
			return paint_viriginia_reel_track_right_quarter_turn_1_tile;
	}

	return NULL;
}