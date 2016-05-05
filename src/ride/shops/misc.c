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
#include "../../paint/supports.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../track_paint.h"

/**
 * rct2: 0x00761378, 0x007614DB, 0x0076163F, 0x007617A5
 */
static void ride_50_52_53_54_paint_setup(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement) {
	bool supportsDrawn = wooden_a_supports_paint_setup(direction & 1, 0, height, RCT2_GLOBAL(0x00F441A4, uint32), NULL);

	RCT2_GLOBAL(0x0141E9D0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9CC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9BC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9B4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9C0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9D4, uint16) = 0xFFFF;

	rct_ride *ride = get_ride(rideIndex);

	rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);

	uint32 image_id = RCT2_GLOBAL(0x00F44198, uint32);
	if (image_id & 0x80000000) {
		image_id &= 0x60FFFFFF;
	}

	image_id += rideEntry->vehicles[0].base_image_id;
	image_id += direction;
	if (supportsDrawn) {
		uint32 ebx = (direction & 1 ? 3396 : 3395) | RCT2_GLOBAL(0x00F441A4, uint32);
		sub_98197C(ebx, 0, 0, 28, 28, 45, height, 2, 2, height, get_current_rotation());

		sub_98199C(image_id, 0, 0, 28, 28, 45, height, 2, 2, height, get_current_rotation());
	} else {
		sub_98197C(image_id, 0, 0, 28, 28, 45, height, 2, 2, height, get_current_rotation());
	}

	height += 48;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x00141E9DA, uint8) = 0x20;
	}
}

/**
 * rct2: 0x00761160
 */
TRACK_PAINT_FUNCTION get_track_paint_function_50_52_53_54(int trackType, int direction) {
	switch (trackType) {
		case 118:
		case 121:
			return ride_50_52_53_54_paint_setup;
	}
	return NULL;
}
