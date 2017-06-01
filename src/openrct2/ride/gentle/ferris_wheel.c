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

static const uint8 edges_1x4_ne_sw[] = {
	EDGE_NW | EDGE_SE,
	EDGE_NW | EDGE_SE | EDGE_NE,
	EDGE_NW | EDGE_SE,
	EDGE_NW | EDGE_SE | EDGE_SW,
};

static const uint8 edges_1x4_nw_se[] = {
	EDGE_NE | EDGE_SW,
	EDGE_NE | EDGE_SW | EDGE_NW,
	EDGE_NE | EDGE_SW,
	EDGE_NE | EDGE_SW | EDGE_SE,
};

typedef struct ferris_wheel_bound_box
{
	sint16 length_x;
	sint16 length_y;
	sint16 offset_x;
	sint16 offset_y;
} ferris_wheel_bound_box;

/** rct2: 0x008A8CA8 */
static ferris_wheel_bound_box ferris_wheel_data[] = {
	{31, 16, 1, 8},
	{16, 31, 8, 1},
	{31, 16, 1, 8},
	{16, 31, 8, 1},
};


/**
 * rct2: 0x004C3874
 */
static void paint_ferris_wheel_structure(uint8 rideIndex, uint8 direction, sint8 axisOffset, uint16 height)
{
	uint32 imageId, baseImageId;

	rct_map_element * savedMapElement = g_currently_drawn_item;

	rct_ride * ride = get_ride(rideIndex);
	rct_ride_entry * rideType = get_ride_entry(ride->subtype);
	rct_vehicle * vehicle = NULL;

	sint8 xOffset = !(direction & 1) ? axisOffset : 0;
	sint8 yOffset = (direction & 1) ? axisOffset : 0;

	height += 7;

	baseImageId = rideType->vehicles[0].base_image_id;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK
	    && ride->vehicles[0] != SPRITE_INDEX_NULL) {
		vehicle = GET_VEHICLE(ride->vehicles[0]);

		gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;
		g_currently_drawn_item = vehicle;
	}

	uint32 imageOffset = 0;
	if (vehicle != NULL) {
		imageOffset = vehicle->vehicle_sprite_type % 8;
	}

	uint32 imageColourFlags = gTrackColours[SCHEME_MISC];
	if (imageColourFlags == 0x20000000) {
		imageColourFlags = ride->vehicle_colours[0].body_colour << 19 | ride->vehicle_colours[0].trim_colour << 24 | 0xA0000000;
	}

	ferris_wheel_bound_box boundBox = ferris_wheel_data[direction];

	imageId = (22150 + (direction & 1) * 2) | gTrackColours[SCHEME_TRACK];
	sub_98197C(imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x, boundBox.offset_y, height, get_current_rotation());

	imageId = (baseImageId + direction * 8 + imageOffset) | imageColourFlags;
	sub_98199C(imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x, boundBox.offset_y, height, get_current_rotation());

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK
	    && ride->vehicles[0] != SPRITE_INDEX_NULL) {
		vehicle = GET_VEHICLE(ride->vehicles[0]);
		for (sint32 i = 0; i < 32; i += 2) {
			if (vehicle->peep[i] == SPRITE_INDEX_NULL) {
				continue;
			}

			rct_peep * peep = GET_PEEP(vehicle->peep[i]);
			if (peep->state != PEEP_STATE_ON_RIDE) {
				continue;
			}

			sint32 frameNum = (vehicle->vehicle_sprite_type + i * 4) % 128;
			imageColourFlags = vehicle->peep_tshirt_colours[i] << 19 | vehicle->peep_tshirt_colours[i + 1] << 24 | 0xA0000000;
			imageId = (baseImageId + 32 + direction * 128 + frameNum) | imageColourFlags;
			sub_98199C(imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x, boundBox.offset_y, height, get_current_rotation());
		}
	}

	imageId = (22150 + (direction & 1) * 2 + 1) | gTrackColours[SCHEME_TRACK];
	sub_98199C(imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x, boundBox.offset_y, height, get_current_rotation());

	g_currently_drawn_item = savedMapElement;
	gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
}


/**
 * rct2: 0x008A8EC4
 */
static void paint_ferris_wheel(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	uint8 relativeTrackSequence = track_map_1x4[direction][trackSequence];

	sint32 edges;
	if (direction & 1) {
		edges = edges_1x4_nw_se[relativeTrackSequence];
	} else {
		edges = edges_1x4_ne_sw[relativeTrackSequence];
	}

	rct_ride * ride = get_ride(rideIndex);
	rct_xy16 position = {gPaintMapPosition.x, gPaintMapPosition.y};

	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_MISC], NULL);

	track_paint_util_paint_floor(edges, gTrackColours[SCHEME_TRACK], height, floorSpritesCork, get_current_rotation());

	uint32 imageId;
	uint8 rotation = get_current_rotation();
	uint32 colourFlags = gTrackColours[SCHEME_MISC];
	if (edges & EDGE_NW && track_paint_util_has_fence(EDGE_NW, position, mapElement, ride, rotation)) {
		imageId = SPR_FENCE_ROPE_NW | colourFlags;
		sub_98199C(imageId, 0, 0, 32, 1, 7, height, 0, 2, height + 2, rotation);
	}
	if (edges & EDGE_NE && track_paint_util_has_fence(EDGE_NE, position, mapElement, ride, rotation)) {
		imageId = SPR_FENCE_ROPE_NE | colourFlags;
		sub_98199C(imageId, 0, 0, 1, 32, 7, height, 2, 0, height + 2, rotation);
	}
	if (edges & EDGE_SE && track_paint_util_has_fence(EDGE_SE, position, mapElement, ride, rotation)) {
		// Bound box is slightly different from track_paint_util_paint_fences
		imageId = SPR_FENCE_ROPE_SE | colourFlags;
		sub_98197C(imageId, 0, 0, 28, 1, 7, height, 0, 29, height + 3, rotation);
	}
	if (edges & EDGE_SW && track_paint_util_has_fence(EDGE_SW, position, mapElement, ride, rotation)) {
		imageId = SPR_FENCE_ROPE_SW | colourFlags;
		sub_98197C(imageId, 0, 0, 1, 32, 7, height, 30, 0, height + 2, rotation);
	}

	switch (relativeTrackSequence) {
		case 1: paint_ferris_wheel_structure(rideIndex, direction, 48, height); break;
		case 2: paint_ferris_wheel_structure(rideIndex, direction, 16, height); break;
		case 0: paint_ferris_wheel_structure(rideIndex, direction, -16, height); break;
		case 3: paint_ferris_wheel_structure(rideIndex, direction, -48, height); break;
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 176, 0x20);
}

/**
 * rct2: 0x008A8CC8
 */
TRACK_PAINT_FUNCTION get_track_paint_function_ferris_wheel(sint32 trackType, sint32 direction)
{
	if (trackType != FLAT_TRACK_ELEM_1_X_4_C) {
		return NULL;
	}

	return paint_ferris_wheel;
}
