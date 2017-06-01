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

#include "../../paint/supports.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../track_paint.h"
#include "../track.h"

/**
 *
 *  rct2: 0x00761378
 *  rct2: 0x007614DB
 *  rct2: 0x0076163F
 *  rct2: 0x007617A5
 */
static void shop_paint_setup(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element* mapElement)
{
	bool hasSupports = wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_3], NULL);

	rct_ride *ride = get_ride(rideIndex);
	rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
	rct_ride_entry_vehicle *firstVehicleEntry = &rideEntry->vehicles[0];

	if (rideEntry == NULL || firstVehicleEntry == NULL)
	{
		log_error("Error drawing shop, rideEntry or firstVehicleEntry is NULL.");
		return;
	}

	uint32 imageId = gTrackColours[SCHEME_TRACK];
	if (imageId & 0x80000000) {
		imageId &= 0x60FFFFFF;
	}
	imageId += firstVehicleEntry->base_image_id;
	imageId += direction;

	if (hasSupports) {
		uint32 foundationImageId = ((direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS) | gTrackColours[SCHEME_3];
		sub_98197C(foundationImageId, 0, 0, 28, 28, 45, height, 2, 2, height, get_current_rotation());

		sub_98199C(imageId, 0, 0, 28, 28, 45, height, 2, 2, height, get_current_rotation());
	} else {
		sub_98197C(imageId, 0, 0, 28, 28, 45, height, 2, 2, height, get_current_rotation());
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/* 0x00761160 */
TRACK_PAINT_FUNCTION get_track_paint_function_shop(sint32 trackType, sint32 direction)
{
	switch (trackType) {
	case FLAT_TRACK_ELEM_1_X_1_A:
	case FLAT_TRACK_ELEM_1_X_1_B: return shop_paint_setup;
	}
	return NULL;
}
