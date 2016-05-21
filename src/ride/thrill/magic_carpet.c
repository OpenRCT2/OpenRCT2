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

#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../track_paint.h"

enum {
	PLANE_BACK,
	PLANE_FRONT,
};

static void paint_magic_carpet_frame(uint8 plane, uint8 direction,
									 rct_xyz16 offset, rct_xyz16 bbOffset, rct_xyz16 bbSize)
{
	uint32 imageId = plane == PLANE_BACK ? 22002 : 22003;
	imageId += (direction & 1) * 2;
	imageId |= RCT2_GLOBAL(0x00F44198, uint32);
	if (plane == PLANE_BACK) {
		sub_98197C(imageId, (sint8)offset.x, (sint8)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y, bbOffset.z, get_current_rotation());
	} else {
		sub_98199C(imageId, (sint8)offset.x, (sint8)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y, bbOffset.z, get_current_rotation());
	}
}

static void paint_magic_carpet_pendulum(uint8 plane, uint32 swingImageId, uint8 direction,
										rct_xyz16 offset, rct_xyz16 bbOffset, rct_xyz16 bbSize)
{
	uint32 imageId = swingImageId;
	if (direction & 2) {
		imageId = (0 - ((sint32)imageId)) & 31;
	}
	if (direction & 1) {
		imageId += 32;
	}
	imageId += plane == PLANE_BACK ? 22006 : 22070;
	imageId |= RCT2_GLOBAL(0x00F44198, uint32);
	sub_98199C(imageId, (sint8)offset.x, (sint8)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y, bbOffset.z, get_current_rotation());
}

/** rct2: 0x00899104 */
static void paint_magic_carpet_structure(rct_ride *ride, uint8 direction, sint8 axisOffset, uint16 height)
{
	height += 7;

	rct_vehicle *vehicle = NULL;
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
		uint16 vehicleSpriteIndex = ride->vehicles[0];
		if (vehicleSpriteIndex != SPRITE_INDEX_NULL) {
			gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;
			vehicle = GET_VEHICLE(vehicleSpriteIndex);
		}
	}

	rct_ride_entry *rideEntry = get_ride_entry_by_ride(ride);
	uint32 vehicleImageId = rideEntry->vehicles[0].base_image_id + direction;
	uint32 dword_1428218 = 0;
	if (vehicle != NULL) {
		dword_1428218 = vehicle->vehicle_sprite_type;
	}

	uint32 dword_1428210 = vehicleImageId;
	uint32 dword_1428214 = direction;

	uint32 imageColourFlags = RCT2_GLOBAL(0x00F441A0, uint32);
	if (imageColourFlags == 0x20000000) {
		imageColourFlags = 0xA0000000 |
			(ride->vehicle_colours[0].trim_colour << 24) |
			(ride->vehicle_colours[0].body_colour << 19);
	}

	uint32 imageId = dword_1428210 | imageColourFlags;
	
	rct_xyz16 offset;
	rct_xyz16 bbOffset;
	rct_xyz16 bbSize;
	
	offset.x = (direction & 1) ? 0 : axisOffset;
	offset.y = (direction & 1) ? axisOffset : 0;
	offset.z = height;
	bbOffset.x = RCT2_ADDRESS(0x014281F4, sint16)[direction * 4];
	bbOffset.y = RCT2_ADDRESS(0x014281F6, sint16)[direction * 4];
	bbOffset.z = height;
	bbSize.x = RCT2_ADDRESS(0x014281F0, sint16)[direction * 4];
	bbSize.y = RCT2_ADDRESS(0x014281F2, sint16)[direction * 4];
	bbSize.z = 127;

	paint_magic_carpet_frame(PLANE_BACK, direction, offset, bbOffset, bbSize);
	paint_magic_carpet_pendulum(PLANE_BACK, dword_1428218, direction, offset, bbOffset, bbSize);

	paint_magic_carpet_pendulum(PLANE_FRONT, dword_1428218, direction, offset, bbOffset, bbSize);
	paint_magic_carpet_frame(PLANE_FRONT, direction, offset, bbOffset, bbSize);

	gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/** rct2: 0x00898514 */
static void paint_magic_carpet(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint8 relativeTrackSequence = track_map_1x4[direction][trackSequence];

	// The end tiles do not have a platform
	switch (relativeTrackSequence) {
	case 0:
	case 2:
		if (direction & 1) {
			metal_a_supports_paint_setup(0, 6, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			metal_a_supports_paint_setup(0, 7, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
		} else {
			metal_a_supports_paint_setup(0, 5, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
			metal_a_supports_paint_setup(0, 8, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
		}

		uint32 imageId = SPR_STATION_BASE_D | RCT2_GLOBAL(0x00F4419C, uint32);
		sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());
		break;
	}

	rct_ride *ride = get_ride(rideIndex);
	switch (relativeTrackSequence) {
	case 3: paint_magic_carpet_structure(ride, direction, -48, height); break;
	case 0: paint_magic_carpet_structure(ride, direction, -16, height); break;
	case 2: paint_magic_carpet_structure(ride, direction,  16, height); break;
	case 1: paint_magic_carpet_structure(ride, direction,  48, height); break;
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 176, 0x20);
}

/**
 *
 *  rct2: 0x00898384
 */
TRACK_PAINT_FUNCTION get_track_paint_function_magic_carpet(int trackType, int direction)
{
	switch (trackType) {
	case 95: return paint_magic_carpet;
	}
	return NULL;
}
