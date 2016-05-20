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

#include "../../config.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../ride_data.h"
#include "../track_data.h"
#include "../track_paint.h"
#include "../../world/map.h"

enum {
	SPR_MINI_GOLF_FLAT_SW_NE = 14404,
	SPR_MINI_GOLF_FLAT_NW_SE = 14405,
	SPR_MINI_GOLF_FLAT_FENCE_BACK_SW_NE = 14406,
	SPR_MINI_GOLF_FLAT_FENCE_BACK_NW_SE = 14407,
	SPR_MINI_GOLF_FLAT_FENCE_FRONT_SW_NE = 14408,
	SPR_MINI_GOLF_FLAT_FENCE_FRONT_NW_SE = 14409,
	SPR_MINI_GOLF_STATION_FLOOR = 14410,
	SPR_MINI_GOLF_TURN_SW_NW = 14411,
	SPR_MINI_GOLF_TURN_NW_NE = 14412,
	SPR_MINI_GOLF_TURN_NE_SE = 14413,
	SPR_MINI_GOLF_TURN_SE_SW = 14413,
};

/** rct2: 0x0087F10C */
static void paint_mini_golf_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;

	if (direction & 1) {
		imageId = SPR_MINI_GOLF_FLAT_NW_SE | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 20, 32, 1, height, 6, 0, height, get_current_rotation());
		paint_util_push_tunnel_right(height, TUNNEL_10);
	} else {
		imageId = SPR_MINI_GOLF_FLAT_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 20, 1, height, 0, 6, height, get_current_rotation());
		paint_util_push_tunnel_left(height, TUNNEL_10);
	}

	metal_a_supports_paint_setup(3, 4, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

	if (RCT2_GLOBAL(0x9DE57C, bool)) {
		rct_map_element * surfaceElement = RCT2_GLOBAL(0x9E3250, rct_map_element *);
		if (surfaceElement->base_height != mapElement->base_height || surfaceElement->properties.surface.slope & 0x1F) {
			if (direction & 1) {
				imageId = SPR_MINI_GOLF_FLAT_FENCE_BACK_NW_SE | RCT2_GLOBAL(0x00F44198, uint32);
				sub_98197C(imageId, 0, 0, 1, 32, 7, height, 10, 0, height + 2, get_current_rotation());

				imageId = SPR_MINI_GOLF_FLAT_FENCE_FRONT_NW_SE | RCT2_GLOBAL(0x00F441A0, uint32);
				sub_98197C(imageId, 0, 0, 1, 32, 7, height, 22, 0, height + 2, get_current_rotation());
			} else {
				imageId = SPR_MINI_GOLF_FLAT_FENCE_BACK_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
				sub_98197C(imageId, 0, 0, 32, 1, 7, height, 0, 10, height + 2, get_current_rotation());

				imageId = SPR_MINI_GOLF_FLAT_FENCE_FRONT_SW_NE | RCT2_GLOBAL(0x00F441A0, uint32);
				sub_98197C(imageId, 0, 0, 32, 1, 7, height, 0, 22, height + 2, get_current_rotation());
			}
		}
	}

	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x */
static void paint_mini_golf_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_ride *ride = get_ride(rideIndex);

	int ebx = ride->entrance_style;
	uint32 flags = 0;


	if (RCT2_GLOBAL(0x00F441A0, uint32) & 0x20000000) {
		flags &= 0x7FFFF;
	}

	RCT2_GLOBAL(0xF441E8, uint32) = flags;
	RCT2_GLOBAL(0xF441E4, uint32) = flags;

	uint32 image_id = SPR_MINI_GOLF_STATION_FLOOR | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(image_id, 0, 0, 32, 28, 1, height, 0, 0, height, get_current_rotation());

	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);
	uint16 entranceLoc = 0;

	uint8 entranceId = (mapElement->properties.track.sequence & 0x70) >> 4;

	if (ride->entrances[entranceId] != entranceLoc && ride->exits[entranceId] != entranceLoc) {
		image_id = SPR_MINI_GOLF_FLAT_FENCE_BACK_SW_NE | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98199C(image_id, 0, -10, 32, 1, 7, height, 0, 0, height + 1, get_current_rotation());

		RCT2_GLOBAL(0xF441E8, uint32)++;
	}

	image_id = SPR_MINI_GOLF_FLAT_FENCE_FRONT_SW_NE | RCT2_GLOBAL(0x00F441A0, uint32);
	sub_98199C(image_id, 0, 10, 32, 1, 7, height, 0, 31, height + 2, get_current_rotation());


	if (RCT2_GLOBAL(0xF441E8, uint32) > 32) {

	}


	// Something with covered stations based on entrance style
	uint32 ebx_2 = RCT2_GLOBAL(0xF441E4, uint32);
	if (RCT2_GLOBAL(0x0141E9DB, uint8) & 3 && ebx_2 >= 32) {
		if (ebx_2 & 0x40000000) {
			image_id = (ebx_2 & 0xBFFFFFFF) + 2;
			sub_98197C(image_id, 0, 0, 32, 32, 0, height, 0, 0, height + 23, get_current_rotation());

			uint32 edi = RCT2_GLOBAL(0x00F44198, uint32);
			// and     edi, (offset map_elements.field_0+19C48h)
			// cmp     edi, (offset map_elements.field_0+99C48h)
			// ja      short loc_87F582

			image_id = (ebx | edi) + 0x380000C + 2;
			sub_98199C(image_id, 0, 0, 32, 32, 0, height, 0, 0, height + 23, get_current_rotation());
		} else {
			image_id = (ebx_2 + 2) | RCT2_GLOBAL(0x00F44198, uint32);
			sub_98197C(image_id, 0, 0, 32, 32, 0, height, 0, 0, height + 23, get_current_rotation());
		}
	}

	wooden_a_supports_paint_setup(0, 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);

	RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9BC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9B4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C0, uint16) = 0xFFFF;

	uint32 eax = 0xFFFF0000 | (height & 0xFF) | (6 << 8);
	RCT2_ADDRESS(0x009E3138, uint32)[RCT2_GLOBAL(0x141F56A, uint8) / 2] = eax;
	RCT2_GLOBAL(0x141F56A, uint16)++;


	height += 32;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 32;
	}
}

/**
 * rct2: 0x00880C23
 */
static void mini_golf_quarter_turn_paint_setup_nw_sw(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	uint32 image_id = SPR_MINI_GOLF_TURN_SW_NW | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(image_id, 0, 0, 26, 24, 1, height, 6, 2, height, get_current_rotation());

	metal_a_supports_paint_setup(3, 4, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));

	RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;

	uint32 eax = 0xFFFF0A00 | ((height / 16) & 0xFF);
	RCT2_ADDRESS(0x009E3138, uint32)[RCT2_GLOBAL(0x141F56A, uint8) / 2] = eax;
	RCT2_GLOBAL(0x141F56A, uint8)++;

	if (RCT2_GLOBAL(0x9DE57C, uint16) & 1) {
		// 0x00880CD0
	}

	height += 32;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x00141E9DA, uint8) = 32;
	}
}


/**
 * rct2: 0x0088129C
 */
static void mini_golf_hole_a_paint_setup_0_0(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {

	bool drewSupports = wooden_a_supports_paint_setup(0, 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);

	RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9BC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9B4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C0, uint16) = 0xFFFF;

	uint32 eax = 0xFFFF0A00 | ((height >> 4) & 0xFF);
	RCT2_ADDRESS(0x009E3138, uint32)[RCT2_GLOBAL(0x141F56A, uint8) / 2] = eax;
	RCT2_GLOBAL(0x141F56A, uint8)++;

	height += 32;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x00141E9DA, uint8) = 32;
	}
	height -= 32;

	uint32 image_id = 14469 | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(image_id, 0, 0, 32, 26, 0, height, 0, 3, height + 24, get_current_rotation());

	if (drewSupports) {
		image_id = 3395 + RCT2_GLOBAL(0x00F4419C, uint32);
		sub_98197C(image_id, 0, 0, 32, 26, 1, height, 0, 3, height, get_current_rotation());
	}

	image_id = 14461 | RCT2_GLOBAL(0x00F44198, uint32);
	if (drewSupports) {
		sub_98199C(image_id, 0, 0, 32, 26, 1, height, 0, 3, height, get_current_rotation());
	} else {
		sub_98197C(image_id, 0, 0, 32, 26, 1, height, 0, 3, height, get_current_rotation());
	}
}

/**
 * rct2: 0x0088125D
 */
static void mini_golf_hole_a_paint_setup_0(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	if (trackSequence == 0) mini_golf_hole_a_paint_setup_0_0(rideIndex, trackSequence, direction, height, mapElement);
}

/**
 * rct2: 0x0087EDC4
 */
TRACK_PAINT_FUNCTION get_track_paint_function_mini_golf(int trackType, int direction) {
	switch (trackType) {
		case TRACK_ELEM_FLAT:
			return paint_mini_golf_track_flat;

		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
		case TRACK_ELEM_END_STATION:
			return paint_mini_golf_station;

		case TRACK_ELEM_25_DEG_UP:
		case TRACK_ELEM_FLAT_TO_25_DEG_UP:
		case TRACK_ELEM_25_DEG_UP_TO_FLAT:

		case TRACK_ELEM_25_DEG_DOWN:
		case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
		case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:

		case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
		case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:

		case TRACK_ELEM_MINI_GOLF_HOLE_A:
		case TRACK_ELEM_MINI_GOLF_HOLE_B:
		case TRACK_ELEM_MINI_GOLF_HOLE_C:
		case TRACK_ELEM_MINI_GOLF_HOLE_D:
		case TRACK_ELEM_MINI_GOLF_HOLE_E:
			return NULL;
	}

	return NULL;
}

/**
 * rct2: 0x006D42F0
 */
void vehicle_visual_mini_golf_player(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, int rct2VehiclePtrFormat)
{
	if (vehicle->num_peeps == 0) {
		return;
	}

	rct_drawpixelinfo *edi = RCT2_GLOBAL(0x0140E9A8, rct_drawpixelinfo*);
	if (edi->zoom_level >= 2) {
		return;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & VIEWPORT_FLAG_INVISIBLE_PEEPS) {
		return;
	}

	rct_ride_entry *rideType = get_ride_entry(get_ride(vehicle->ride)->subtype);
	rct_sprite *sprite = &g_sprite_list[vehicle->peep[0]];

	uint32 eax = RCT2_ADDRESS(0x008B8F74, uint32)[vehicle->var_D4];
	uint32 ebx = (RCT2_GLOBAL(eax + vehicle->var_C5, uint8) << 2) + (imageDirection >> 3);

	uint32 image_id = rideType->vehicles[0].base_image_id + 1 + ebx;
	uint32 peep_palette = sprite->peep.tshirt_colour << 19 | sprite->peep.trousers_colour << 24 | 0x0A0000000;
	sub_98197C(image_id | peep_palette, 0, 0, 1, 1, 11, z, 0, 0, z + 5, get_current_rotation());
}

/**
 * rct2: 0x006D43C6
 */
void vehicle_visual_mini_golf_ball(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, int rct2VehiclePtrFormat)
{
	if (vehicle->var_D4 != 1) {
		return;
	}

	rct_drawpixelinfo *edi = RCT2_GLOBAL(0x0140E9A8, rct_drawpixelinfo*);
	if (edi->zoom_level >= 1) {
		return;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & VIEWPORT_FLAG_INVISIBLE_PEEPS) {
		return;
	}

	rct_ride *ride = get_ride(vehicle->ride);
	rct_ride_entry *rideType = get_ride_entry(ride->subtype);
	//log_info("base_image_id: %d, image_id: %d", rideType->vehicles[0].base_image_id, rideType->vehicles[0].base_image_id & 0x7FFFF);

	uint32 image_id = rideType->vehicles[0].base_image_id;
	sub_98197C(image_id, 0, 0, 1, 1, 0, z, 0, 0, z + 3, get_current_rotation());
}
