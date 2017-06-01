#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../track_paint.h"
#include "../track.h"

typedef struct rct_crooked_house_bound_box {
	sint16 offset_x;
	sint16 offset_y;
	sint16 length_x;
	sint16 length_y;
} rct_crooked_house_bound_box;

rct_crooked_house_bound_box crooked_house_data[] = {
	{6,   0,   42, 24},
	{0,   0,   0,  0},
	{-16, -16, 32, 32},
	{0,   0,   0,  0}, // Unused
	{0,   6,   24, 42}
};

/**
 * rct2: 0x0088ABA4
 * @param (edi) direction
 * @param (al) al
 * @param (cl) cl
 * @param (ebx) image_id
 * @param (edx) height
 */
static void sub_88ABA4(uint8 direction, uint8 x_offset, uint8 y_offset, uint32 segment, sint32 height) {
	rct_map_element *original_map_element = g_currently_drawn_item;

	rct_ride *ride = get_ride(original_map_element->properties.track.ride_index);

	rct_ride_entry *ride_type = get_ride_entry(ride->subtype);

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
		if (ride->vehicles[0] != (uint16)-1) {
			rct_sprite *sprite = get_sprite(ride->vehicles[0]);
			gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;
			g_currently_drawn_item = sprite;
		}
	}


	uint32 image_id = (direction + ride_type->vehicles[0].base_image_id) | gTrackColours[SCHEME_MISC];

	rct_crooked_house_bound_box boundBox = crooked_house_data[segment];
	sub_98197C(image_id, x_offset, y_offset, boundBox.length_x, boundBox.length_y, 127, height + 3, boundBox.offset_x, boundBox.offset_y, height + 3, get_current_rotation());
}

static void paint_crooked_house(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	trackSequence = track_map_3x3[direction][trackSequence];

	sint32 edges = edges_3x3[trackSequence];
	rct_ride * ride = get_ride(rideIndex);
	rct_xy16 position = {gPaintMapPosition.x, gPaintMapPosition.y};

	wooden_a_supports_paint_setup((direction & 1), 0, height, gTrackColours[SCHEME_MISC], NULL);

	track_paint_util_paint_floor(edges, gTrackColours[SCHEME_TRACK], height, floorSpritesCork, get_current_rotation());

	track_paint_util_paint_fences(edges, position, mapElement, ride, gTrackColours[SCHEME_MISC], height, fenceSpritesRope, get_current_rotation());

	switch(trackSequence) {
		case 3: sub_88ABA4(direction, 32, 224, 0, height); break;
		// case 5: sub_88ABA4(direction, 0, 224, 1, height); break;
		case 6: sub_88ABA4(direction, 224, 32, 4, height); break;
		case 7: sub_88ABA4(direction, 224, 224, 2, height); break;
		// case 8: sub_88ABA4(rideIndex, 224, 0, 3, height); break;
	}

	sint32 cornerSegments = 0;
	switch (trackSequence) {
		case 1:
			// Top
			cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC;
			break;
		case 3:
			// Right
			cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4;
			break;
		case 6:
			// Left
			cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0;
			break;
		case 7:
			// Bottom
			cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4;
			break;
	}

	paint_util_set_segment_support_height(cornerSegments, height + 2, 0x20);
	paint_util_set_segment_support_height(SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 128, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_crooked_house(sint32 trackType, sint32 direction) {
	if (trackType != FLAT_TRACK_ELEM_3_X_3) {
		return NULL;
	}

	return paint_crooked_house;
}
