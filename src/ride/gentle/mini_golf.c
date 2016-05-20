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

/* rct2: 0x007667AE */
static rct_xy16 loc_7667AE[] = {
	{ .x = 0,  .y = -1 },
	{ .x = 1,  .y = 0 },
	{ .x = 0,  .y = 1},
	{ .x = -1, .y = 0 },
};

/* rct2: 0x007667AC */
static rct_xy16 loc_7667AC[] = {
	{ .x = -1, .y = 0 },
	{ .x = 0,  .y = -1 },
	{ .x = 1,  .y = 0 },
	{ .x = 0,  .y = 1 },
};

enum {
	SPR_MINI_GOLF_FLAT_NE_SW = 14404,
	SPR_MINI_GOLF_FLAT_NW_SE,
	SPR_MINI_GOLF_FLAT_FENCE_BACK_NE_SW = 14406,
	SPR_MINI_GOLF_FLAT_FENCE_BACK_NW_SE,
	SPR_MINI_GOLF_FLAT_FENCE_FRONT_NE_SW = 14408,
	SPR_MINI_GOLF_FLAT_FENCE_FRONT_NW_SE,
	SPR_MINI_GOLF_STATION_FLOOR = 14410,
	SPR_MINI_GOLF_TURN_NW_SW = 14411,
};

/**
 * rct2: 0x0087FA27
 */
static void mini_golf_flat_paint_setup_rot_0_2(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	uint32 image_id = SPR_MINI_GOLF_FLAT_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(image_id, 0, 0, 32, 20, 1, height, 0, 0, height, get_current_rotation());

	metal_a_supports_paint_setup(3, 4, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));


	RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;

	uint32 eax = 0xFFFFFFFF | ((height / 16) & 0xFF);
	RCT2_ADDRESS(0x009E3138, uint32)[RCT2_GLOBAL(0x141F56A, uint8) / 2] = eax;
	RCT2_GLOBAL(0x141F56A, uint8)++;

	bool drawFences = false;

	if (RCT2_GLOBAL(0x9DE57C, uint16) & 1) {
		rct_map_element *highlight = RCT2_ADDRESS(0x009E3250, rct_map_element);

		if (mapElement->base_height != highlight->base_height) {
			drawFences = true;
		} else {
			// test    byte ptr [edi+4], 1Fh
			// jz      loc_87FB74
		}
	}

	if (drawFences) {
		image_id = SPR_MINI_GOLF_FLAT_FENCE_BACK_NE_SW | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98197C(image_id, 0, 0, 32, 1, 7, height, 0, 10, height + 2, get_current_rotation());

		image_id = SPR_MINI_GOLF_FLAT_FENCE_FRONT_NE_SW | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98197C(image_id, 0, 0, 32, 1, 7, height, 0, 22, height + 2, get_current_rotation());
	}

	height += 32;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 32;
	}
}

/**
 * rct2: 0x0087FB91
 */
static void mini_golf_flat_paint_setup_rot_1_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
}

/**
 * rct2: 0x0087F22C
 */
static void mini_golf_station_paint_setup_rot_0_1(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	rct_ride *ride = get_ride(rideIndex);

	int ebx = ride->entrance_style;
	uint32 flags = RideEntranceDefinitions[ride->entrance_style].flags;


	if (RCT2_GLOBAL(0x00F441A0, uint32) & 0x20000000) {
		flags &= 0x7FFFF;
	}

	RCT2_GLOBAL(0xF441E8, uint32) = flags;
	RCT2_GLOBAL(0xF441E4, uint32) = flags;

	uint32 image_id = SPR_MINI_GOLF_STATION_FLOOR | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(image_id, 0, 0, 32, 28, 1, height, 0, 0, height, get_current_rotation());

	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);
	uint16 entranceLoc =
		((x / 32) + loc_7667AE[get_current_rotation()].x) |
		(((y / 32) + loc_7667AE[get_current_rotation()].y) << 8);

	uint8 entranceId = (mapElement->properties.track.sequence & 0x70) >> 4;

	if (ride->entrances[entranceId] != entranceLoc && ride->exits[entranceId] != entranceLoc) {
		image_id = SPR_MINI_GOLF_FLAT_FENCE_BACK_NE_SW | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98199C(image_id, 0, -10, 32, 1, 7, height, 0, 0, height + 1, get_current_rotation());

		RCT2_GLOBAL(0xF441E8, uint32)++;
	}

	image_id = SPR_MINI_GOLF_FLAT_FENCE_FRONT_NE_SW | RCT2_GLOBAL(0x00F441A0, uint32);
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
 * rct2: 0x0087F625
 */
static void mini_golf_station_paint_setup_rot_1_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
}

/**
 * rct2: 0x00880C23
 */
static void mini_golf_quarter_turn_paint_setup_nw_sw(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	uint32 image_id = SPR_MINI_GOLF_TURN_NW_SW | RCT2_GLOBAL(0x00F44198, uint32);
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


static void get_color_name(int color, char *buffer) {
	char *COLOUR_NAMES[] = {
		"COLOUR_BLACK",
		"COLOUR_GREY",
		"COLOUR_WHITE",
		"COLOUR_DARK_PURPLE",
		"COLOUR_LIGHT_PURPLE",
		"COLOUR_BRIGHT_PURPLE",
		"COLOUR_DARK_BLUE",
		"COLOUR_LIGHT_BLUE",
		"COLOUR_ICY_BLUE",
		"COLOUR_TEAL",
		"COLOUR_AQUAMARINE",
		"COLOUR_SATURATED_GREEN",
		"COLOUR_DARK_GREEN",
		"COLOUR_MOSS_GREEN",
		"COLOUR_BRIGHT_GREEN",
		"COLOUR_OLIVE_GREEN",
		"COLOUR_DARK_OLIVE_GREEN",
		"COLOUR_BRIGHT_YELLOW",
		"COLOUR_YELLOW",
		"COLOUR_DARK_YELLOW",
		"COLOUR_LIGHT_ORANGE",
		"COLOUR_DARK_ORANGE",
		"COLOUR_LIGHT_BROWN",
		"COLOUR_SATURATED_BROWN",
		"COLOUR_DARK_BROWN",
		"COLOUR_SALMON_PINK",
		"COLOUR_BORDEAUX_RED",
		"COLOUR_SATURATED_RED",
		"COLOUR_BRIGHT_RED",
		"COLOUR_DARK_PINK",
		"COLOUR_BRIGHT_PINK",
		"COLOUR_LIGHT_PINK",
	};

	sprintf(buffer, "%s", COLOUR_NAMES[color]);
}

static void log_color(uint32 color) {
	return;
	int image_id = color & 0x7ffff;
	int color_1 = (color >> 19) & 31;
	int color_2 = (color >> 24) & 31;
	int flags = color >> 29;

	char color_1_name[128] = {0};
	get_color_name(color_1, color_1_name);
	char color_2_name[128] = {0};
	get_color_name(color_2, color_2_name);

	printf("image:%d, color A: %s, colorB: %s, flags:%X\n", image_id, color_1_name, color_2_name, flags);
}

/**
 * rct2: 0x0088129C
 */
static void mini_golf_hole_a_paint_setup_0_0(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {

	//printf("----\n");
	log_color(RCT2_GLOBAL(0x00F44198, uint32));
	log_color(RCT2_GLOBAL(0x00F441A0, uint32));
	log_color(RCT2_GLOBAL(0x00F441A4, uint32));
	log_color(RCT2_GLOBAL(0x00F4419C, uint32));

	log_color(RCT2_ADDRESS(0x00993CC4, uint32)[gConfigGeneral.construction_marker_colour]);


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
			if (direction == 0 || direction == 2) {
				return mini_golf_flat_paint_setup_rot_0_2;
			}
			return mini_golf_flat_paint_setup_rot_1_3;

		case TRACK_ELEM_25_DEG_UP:
			// 0x0087F11C
			break;
		case TRACK_ELEM_FLAT_TO_25_DEG_UP:
			// 0x0087F12C
			break;
		case TRACK_ELEM_25_DEG_UP_TO_FLAT:
			// 0x0087F13C
			break;
		case TRACK_ELEM_25_DEG_DOWN:
			// 0x0087F14C
			break;
		case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
			// 0x0087F15C
			break;
		case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
			// 0x0087F16C
			break;
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
		case TRACK_ELEM_END_STATION:
			if (direction == 0 || direction == 2) {
				return mini_golf_station_paint_setup_rot_0_1;
			}

			return mini_golf_station_paint_setup_rot_1_3;

		case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
			if (direction == 0) return mini_golf_quarter_turn_paint_setup_nw_sw;
			// 0x0087F1AC
			break;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
			if (direction == 1) return mini_golf_quarter_turn_paint_setup_nw_sw;
			// 0x0087F1BC
			break;
		case TRACK_ELEM_MINI_GOLF_HOLE_A:
			if (direction == 0) return mini_golf_hole_a_paint_setup_0;
			// 0x0087F1CC
			break;
		case TRACK_ELEM_MINI_GOLF_HOLE_B:
			// 0x0087F1DC
			break;
		case TRACK_ELEM_MINI_GOLF_HOLE_C:
			// 0x0087F1EC
			break;
		case TRACK_ELEM_MINI_GOLF_HOLE_D:
			// 0x0087F1FC
			break;
		case TRACK_ELEM_MINI_GOLF_HOLE_E:
			// 0x0087F20C
			break;
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
