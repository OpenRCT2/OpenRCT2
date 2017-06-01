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

/** rct2: 0x01428010 */
static const uint32 swinging_inverter_ship_base_sprite_offset[] = {
	0,16,0,16
};

/** rct2: 0x01428020 */
static const uint32 swinging_inverter_ship_animating_base_sprite_offset[] = {
	32,33,32,33
};

typedef struct swinging_inverter_ship_bound_box
{
	sint16 length_x;
	sint16 length_y;
	sint16 offset_x;
	sint16 offset_y;
} swinging_inverter_ship_bound_box;

/** rct2: 0x01428020 */
static const swinging_inverter_ship_bound_box swinging_inverter_ship_bounds[] = {
	{32, 16, 0, 8},
	{16, 32, 8, 0},
	{32, 16, 0, 8},
	{16, 32, 8, 0}
};

enum {
	SPR_SWINGING_INVERTER_SHIP_FRAME_0 = 21998,
	SPR_SWINGING_INVERTER_SHIP_FRAME_1 = 21999,
	SPR_SWINGING_INVERTER_SHIP_FRAME_2 = 22000,
	SPR_SWINGING_INVERTER_SHIP_FRAME_3 = 22001,
};

static const uint32 swinging_inverter_ship_frame_sprites[] = {
	SPR_SWINGING_INVERTER_SHIP_FRAME_0,
	SPR_SWINGING_INVERTER_SHIP_FRAME_1,
	SPR_SWINGING_INVERTER_SHIP_FRAME_2,
	SPR_SWINGING_INVERTER_SHIP_FRAME_3
};

static void paint_swinging_inverter_ship_structure(rct_ride * ride, uint8 direction, sint8 axisOffset, uint16 height)
{
	rct_map_element * savedMapElement = g_currently_drawn_item;

	rct_ride_entry * rideType = get_ride_entry(ride->subtype);
	rct_vehicle * vehicle = NULL;

	sint8 xOffset = !(direction & 1) ? axisOffset : 0;
	sint8 yOffset = (direction & 1) ? axisOffset : 0;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK
	    && ride->vehicles[0] != SPRITE_INDEX_NULL) {
		vehicle = GET_VEHICLE(ride->vehicles[0]);

		gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;
		g_currently_drawn_item = vehicle;
	}

	uint32 vehicleImageId = rideType->vehicles[0].base_image_id + swinging_inverter_ship_base_sprite_offset[direction];
	if (vehicle != NULL) {
		sint32 rotation = (sint8) vehicle->vehicle_sprite_type;
		if (rotation != 0) {
			vehicleImageId = rideType->vehicles[0].base_image_id + swinging_inverter_ship_animating_base_sprite_offset[direction];

			if (direction & 2) {
				rotation = -rotation;
			}
			if (rotation < 0) {
				rotation += 72;
			}

			vehicleImageId += (rotation - 1) << 1;
		}
	}

	uint32 colourFlags = gTrackColours[SCHEME_MISC];
	if (colourFlags == 0x20000000) {
		colourFlags = ride->vehicle_colours[0].body_colour << 19 | ride->vehicle_colours[0].trim_colour << 24 | 0xA0000000;
	}

	swinging_inverter_ship_bound_box boundBox = swinging_inverter_ship_bounds[direction];
	vehicleImageId = vehicleImageId | colourFlags;
	uint32 frameImageId = swinging_inverter_ship_frame_sprites[direction] | gTrackColours[SCHEME_TRACK];

	if (direction & 2) {
		sub_98197C(vehicleImageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x, boundBox.offset_y, height, get_current_rotation());
		sub_98199C(frameImageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x, boundBox.offset_y, height, get_current_rotation());
	} else {
		sub_98197C(frameImageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x, boundBox.offset_y, height, get_current_rotation());
		sub_98199C(vehicleImageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x, boundBox.offset_y, height, get_current_rotation());
	}

	g_currently_drawn_item = savedMapElement;
	gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/** rct2: 0x00760260 */
static void paint_swinging_inverter_ship(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	uint8 relativeTrackSequence = track_map_1x4[direction][trackSequence];

	rct_ride * ride = get_ride(rideIndex);

	uint32 imageId;

	if (relativeTrackSequence != 1 && relativeTrackSequence != 3) {
		if (direction & 1) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		} else {
			metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		}

		imageId = SPR_STATION_BASE_D | gTrackColours[SCHEME_SUPPORTS];
		sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());

		switch (direction) {
			case 0:
				imageId = SPR_STATION_PLATFORM_SW_NE | gTrackColours[SCHEME_TRACK];
				sub_98196C(imageId, 0, 24, 32, 8, 1, height + 9, get_current_rotation());
				break;
			case 1:
				imageId = SPR_STATION_PLATFORM_NW_SE | gTrackColours[SCHEME_TRACK];
				sub_98196C(imageId, 24, 0, 8, 32, 1, height + 9, get_current_rotation());
				break;
			case 2:
				imageId = SPR_STATION_PLATFORM_SW_NE | gTrackColours[SCHEME_TRACK];
				sub_98199C(imageId, 0, 0, 32, 8, 1, height + 9, -2, 0, height, get_current_rotation());
				break;
			case 3:
				imageId = SPR_STATION_PLATFORM_NW_SE | gTrackColours[SCHEME_TRACK];
				sub_98199C(imageId, 0, 0, 8, 32, 1, height + 9, 0, -2, height, get_current_rotation());
				break;
		}
	}

	switch (relativeTrackSequence) {
		case 1: paint_swinging_inverter_ship_structure(ride, direction, 48, height + 7); break;
		case 2: paint_swinging_inverter_ship_structure(ride, direction, 16, height + 7); break;
		case 0: paint_swinging_inverter_ship_structure(ride, direction, -16, height + 7); break;
		case 3: paint_swinging_inverter_ship_structure(ride, direction, -48, height + 7); break;
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 176, 0x20);
}


/**
 * rct2: 0x00760070
 */
TRACK_PAINT_FUNCTION get_track_paint_function_swinging_inverter_ship(sint32 trackType, sint32 direction)
{
	if (trackType != FLAT_TRACK_ELEM_1_X_4_B) {
		return NULL;
	}

	return paint_swinging_inverter_ship;
}
