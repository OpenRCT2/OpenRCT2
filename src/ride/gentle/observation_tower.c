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
#include "../../paint/paint.h"
#include "../track_paint.h"
#include "../track.h"
#include "../../world/map.h"
#include "../../paint/supports.h"

enum
{
	SPR_OBSERVATION_TOWER_SEGMENT_BASE = 14986,
	SPR_OBSERVATION_TOWER_SEGMENT = 14987,
	SPR_OBSERVATION_TOWER_SEGMENT_TOP = 14988,
};

/**
 *
 *  rct2: 0x006D6258
 */
void vehicle_visual_observation_tower(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	int image_id;
	int baseImage_id = (vehicle->restraints_position / 64);
	if (vehicle->restraints_position >= 64) {
		if ((imageDirection / 8) && (imageDirection / 8) != 3) {
			baseImage_id *= 2;
			baseImage_id += vehicleEntry->base_image_id + 28;
			if ((imageDirection / 8) != 1) {
				baseImage_id -= 6;
			}
		} else {
			baseImage_id = vehicleEntry->base_image_id + 8;
		}
	} else {
		baseImage_id = (vehicle->var_C5 * 2) + vehicleEntry->base_image_id + 8;
	}

	image_id = baseImage_id | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0x80000000;
	paint_struct* ps = sub_98197C(image_id, 0, 0, 2, 2, 41, z, -11, -11, z + 1, get_current_rotation());
	if (ps != NULL) {
		ps->tertiary_colour = vehicle->colours_extended;
	}

	image_id++;

	ps = sub_98197C(image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1, get_current_rotation());
	if (ps != NULL) {
		ps->tertiary_colour = vehicle->colours_extended;
	}

	assert(vehicleEntry->effect_visual == 1);
}

/** rct2: 0x0070DD6C */
static void paint_observation_tower_base(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = track_map_3x3[direction][trackSequence];

	int edges = edges_3x3[trackSequence];
	rct_ride * ride = get_ride(rideIndex);
	rct_xy16 position = {RCT2_GLOBAL(0x009DE56A, sint16), RCT2_GLOBAL(0x009DE56E, sint16)};

	wooden_a_supports_paint_setup((direction & 1), 0, height, RCT2_GLOBAL(0x00F441A0, uint32), NULL);

	uint32 imageId = SPR_FLOOR_METAL_B | RCT2_GLOBAL(0x00F4419C, uint32);
	sub_98197C(imageId, 0, 0, 32, 32, 1, height, 0, 0, height, get_current_rotation());

	track_paint_util_paint_fences(edges, position, mapElement, ride, RCT2_GLOBAL(0x00F44198, uint32), height, fenceSpritesMetalB, get_current_rotation());

	if (trackSequence == 0) {
		imageId = SPR_OBSERVATION_TOWER_SEGMENT_BASE | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 2, 2, 27, height, 8, 8, height + 3, get_current_rotation());

		height += 32;
		imageId = SPR_OBSERVATION_TOWER_SEGMENT | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 2, 2, 30, height, 8, 8, height, get_current_rotation());

		height += 32;
		imageId = SPR_OBSERVATION_TOWER_SEGMENT | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 2, 2, 30, height, 8, 8, height, get_current_rotation());

		RCT2_GLOBAL(0x9E323C, uint16) = (((height + 32) >> 4) & 0x00FF) | (6 << 8);
	}

	int blockedSegments = 0;
	switch (trackSequence) {
		case 0: blockedSegments = SEGMENTS_ALL; break;
		case 1: blockedSegments = SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC; break;
		case 2: blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC; break;
		case 3: blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0; break;
		case 4: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8; break;
		case 5: blockedSegments = SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0; break;
		case 6: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0; break;
		case 7: blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
		case 8: blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0; break;
	}
	paint_util_set_segment_support_height(blockedSegments, 0xFFFF, 0);
	paint_util_set_segment_support_height(SEGMENTS_ALL & ~blockedSegments, height + 2, 0x20);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0070DD7C */
static void paint_observation_tower_section(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (trackSequence == 1) {
		return;
	}

	uint32 imageId = SPR_OBSERVATION_TOWER_SEGMENT | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(imageId, 0, 0, 2, 2, 30, height, 8, 8, height, get_current_rotation());

	rct_map_element * nextMapElement = mapElement + 1;
	if (map_element_is_last_for_tile(mapElement) || mapElement->clearance_height != nextMapElement->base_height) {
		uint32 imageId = SPR_OBSERVATION_TOWER_SEGMENT_TOP | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98199C(imageId, 0, 0, 2, 2, 30, height, 8, 8, height, get_current_rotation());
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);

	RCT2_GLOBAL(0x9E323C, uint16) = (((height + 32) >> 4) & 0x00FF) | (6 << 8);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/**
 * rct2: 0x0070DC5C
 */
TRACK_PAINT_FUNCTION get_track_paint_function_observation_tower(int trackType, int direction)
{
	switch (trackType) {
		case TRACK_ELEM_TOWER_BASE:
			return paint_observation_tower_base;

		case TRACK_ELEM_TOWER_SECTION:
			return paint_observation_tower_section;
	}

	return NULL;
}
