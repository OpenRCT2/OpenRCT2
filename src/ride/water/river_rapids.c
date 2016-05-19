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

#include "../../addresses.h"
#include "../../config.h"
#include "../../interface/viewport.h"
#include "../../world/sprite.h"
#include "../../paint/paint.h"
#include "../vehicle_paint.h"
#include "../../game.h"
#include "../track_paint.h"
#include "../track.h"
#include "../../paint/supports.h"

// 0x0099279E:
static const vehicle_boundbox _riverRapidsBoundbox[] = {
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 }
};

enum
{
	SPR_RIVER_RAPIDS_FLAT_SW_NE = 21132,
	SPR_RIVER_RAPIDS_FLAT_NW_SE = 21133,
	SPR_RIVER_RAPIDS_FLAT_NE_SW = 21134,
	SPR_RIVER_RAPIDS_FLAT_SE_NW = 21135,
	SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE = 21136,
	SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE = 21137,
	SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW = 21138,
	SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW = 21139,
	SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SW_NE = 21140,
	SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NW_SE = 21141,
	SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NE_SW = 21142,
	SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SE_NW = 21143,
	SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 21144,
	SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 21145,
	SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 21146,
	SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 21147,

	SPR_RIVER_RAPIDS_25_DEG_UP_SW_NE = 21156,
	SPR_RIVER_RAPIDS_25_DEG_UP_NW_SE = 21157,
	SPR_RIVER_RAPIDS_25_DEG_UP_NE_SW = 21158,
	SPR_RIVER_RAPIDS_25_DEG_UP_SE_NW = 21159,
	SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SW_NE = 21160,
	SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NW_SE = 21161,
	SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NE_SW = 21162,
	SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SE_NW = 21163,
};

/**
 *
 *  rct2: 0x006D5889
 */
void vehicle_visual_river_rapids(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	int image_id;
	int baseImage_id = imageDirection;
	uint32 rotation = get_current_rotation();
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

	const vehicle_boundbox *bb = &_riverRapidsBoundbox[j];
	image_id = baseImage_id | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0xA0000000;
	sub_98197C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);

	if (RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*)->zoom_level < 2 && vehicle->num_peeps > 0) {
		// Draw peeps: (this particular vehicle doesn't sort them back to front like others so the back ones sometimes clip, but thats how the original does it...)
		int peeps = ((ecx / 8) + 0) & 3;
		image_id = (baseImage_id + ((peeps + 1) * 72)) | (vehicle->peep_tshirt_colours[0] << 19) | (vehicle->peep_tshirt_colours[1] << 24) | 0xA0000000;
		sub_98199C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
		if (vehicle->num_peeps > 2) {
			peeps = ((ecx / 8) + 2) & 3;
			image_id = (baseImage_id + ((peeps + 1) * 72)) | (vehicle->peep_tshirt_colours[2] << 19) | (vehicle->peep_tshirt_colours[3] << 24) | 0xA0000000;
			sub_98199C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
		}
		if (vehicle->num_peeps > 4) {
			peeps = ((ecx / 8) + 1) & 3;
			image_id = (baseImage_id + ((peeps + 1) * 72)) | (vehicle->peep_tshirt_colours[4] << 19) | (vehicle->peep_tshirt_colours[5] << 24) | 0xA0000000;
			sub_98199C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
		}
		if (vehicle->num_peeps > 6) {
			peeps = ((ecx / 8) + 3) & 3;
			image_id = (baseImage_id + ((peeps + 1) * 72)) | (vehicle->peep_tshirt_colours[6] << 19) | (vehicle->peep_tshirt_colours[7] << 24) | 0xA0000000;
			sub_98199C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
		}
	}

	vehicle_visual_splash_effect(z, vehicle, vehicleEntry);
}

/** rct2: 0x00757650 */
static void paint_river_rapids_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;

	if (direction & 1) {
		imageId = (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_NW_SE : SPR_RIVER_RAPIDS_FLAT_SE_NW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());

		imageId = (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE : SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 1, 32, 3, height, 27, 0, height + 17, get_current_rotation());
	} else {
		imageId = (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_SW_NE : SPR_RIVER_RAPIDS_FLAT_NE_SW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());

		imageId = (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE : SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 1, 3, height, 0, 27, height + 17, get_current_rotation());
	}

	wooden_a_supports_paint_setup((direction & 1), 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);

	if (direction & 1) {
		paint_util_push_tunnel_right(height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_left(height, TUNNEL_6);
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x */
static void paint_river_rapids_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/** rct2: 0x00757660 */
static void paint_river_rapids_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;
	paint_struct * ps;

	switch (direction) {
		case 0:
			imageId = SPR_RIVER_RAPIDS_25_DEG_UP_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 32, 24, 4, height, 0, 4, height, get_current_rotation());

			imageId = SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 32, 1, 34, height, 0, 27, height + 16, get_current_rotation());

			wooden_a_supports_paint_setup(0, 9, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height - 8, TUNNEL_7);
			break;

		case 1:
			imageId = SPR_RIVER_RAPIDS_25_DEG_UP_NW_SE | RCT2_GLOBAL(0x00F44198, uint32);
			ps = sub_98197C(imageId, 0, 0, 24, 32, 4, height, 4, 0, height, get_current_rotation());
			RCT2_GLOBAL(0x009DEA58, paint_struct*) = ps;

			imageId = SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NW_SE | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 1, 32, 34, height, 27, 0, height + 16, get_current_rotation());

			wooden_a_supports_paint_setup(1, 10, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height + 8, TUNNEL_8);
			break;

		case 2:
			imageId = SPR_RIVER_RAPIDS_25_DEG_UP_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
			ps = sub_98197C(imageId, 0, 0, 32, 24, 4, height, 0, 4, height, get_current_rotation());
			RCT2_GLOBAL(0x009DEA58, paint_struct*) = ps;

			imageId = SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 32, 1, 34, height, 0, 27, height + 16, get_current_rotation());

			wooden_a_supports_paint_setup(0, 11, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height + 8, TUNNEL_8);
			break;

		case 3:
			imageId = SPR_RIVER_RAPIDS_25_DEG_UP_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 24, 32, 4, height, 4, 0, height, get_current_rotation());

			imageId = SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 1, 32, 34, height, 27, 0, height + 16, get_current_rotation());

			wooden_a_supports_paint_setup(1, 12, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height - 8, TUNNEL_7);
			break;
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x00757670 */
static void paint_river_rapids_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;
	paint_struct * ps;

	switch (direction) {
		case 0:
			imageId = SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());

			imageId = SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 32, 1, 26, height, 0, 27, height + 16, get_current_rotation());

			wooden_a_supports_paint_setup(0, 1, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;

		case 1:
			imageId = SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NW_SE | RCT2_GLOBAL(0x00F44198, uint32);
			ps = sub_98197C(imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());
			RCT2_GLOBAL(0x009DEA58, paint_struct*) = ps;

			imageId = SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NW_SE | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 1, 32, 26, height, 27, 0, height + 16, get_current_rotation());

			wooden_a_supports_paint_setup(1, 2, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height, TUNNEL_8);
			break;

		case 2:
			imageId = SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
			ps = sub_98197C(imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());
			RCT2_GLOBAL(0x009DEA58, paint_struct*) = ps;

			imageId = SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 32, 1, 26, height, 0, 27, height + 16, get_current_rotation());

			wooden_a_supports_paint_setup(0, 3, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_left(height, TUNNEL_8);
			break;

		case 3:
			imageId = SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());

			imageId = SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(imageId, 0, 0, 1, 32, 26, height, 27, 0, height + 16, get_current_rotation());

			wooden_a_supports_paint_setup(1, 4, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_force_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x */
static void paint_river_rapids_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/** rct2: 0x */
static void paint_river_rapids_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/** rct2: 0x */
static void paint_river_rapids_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/** rct2: 0x */
static void paint_river_rapids_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/** rct2: 0x */
static void paint_river_rapids_track_left_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/** rct2: 0x */
static void paint_river_rapids_track_right_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/** rct2: 0x */
static void paint_river_rapids_track_waterfall(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/** rct2: 0x */
static void paint_river_rapids_track_rapids(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/** rct2: 0x */
static void paint_river_rapids_track_on_ride_photo(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/** rct2: 0x */
static void paint_river_rapids_track_whirlpool(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

}

/**
 * rct2: 0x0075745C
 **/
TRACK_PAINT_FUNCTION get_track_paint_function_river_rapids(int trackType, int direction)
{
	switch (trackType) {
		case TRACK_ELEM_FLAT:
			return paint_river_rapids_track_flat;

		case TRACK_ELEM_END_STATION:
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
			return paint_river_rapids_station;

		case TRACK_ELEM_25_DEG_UP:
			return paint_river_rapids_track_25_deg_up;
		case TRACK_ELEM_FLAT_TO_25_DEG_UP:
			return paint_river_rapids_track_flat_to_25_deg_up;
		case TRACK_ELEM_25_DEG_UP_TO_FLAT:
			return paint_river_rapids_track_25_deg_up_to_flat;

		case TRACK_ELEM_25_DEG_DOWN:
			return paint_river_rapids_track_25_deg_down;
		case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
			return paint_river_rapids_track_flat_to_25_deg_down;
		case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
			return paint_river_rapids_track_25_deg_down_to_flat;

		case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
			return paint_river_rapids_track_left_quarter_turn_1_tile;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
			return paint_river_rapids_track_right_quarter_turn_1_tile;

		case TRACK_ELEM_WATERFALL:
			return paint_river_rapids_track_waterfall;

		case TRACK_ELEM_RAPIDS:
			return paint_river_rapids_track_rapids;

		case TRACK_ELEM_ON_RIDE_PHOTO:
			return paint_river_rapids_track_on_ride_photo;

		case TRACK_ELEM_WHIRLPOOL:
			return paint_river_rapids_track_whirlpool;
	}

	return NULL;
}
