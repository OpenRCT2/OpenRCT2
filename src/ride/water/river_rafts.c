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
#include "../../paint/supports.h"
#include "../../paint/paint.h"
#include "../track.h"
#include "../track_paint.h"

enum {
	SPR_RIVER_RAFTS_FLAT_SW_NE			= 20820,
	SPR_RIVER_RAFTS_FLAT_NW_SE			= 20821,
	SPR_RIVER_RAFTS_FLAT_NE_SW			= 20822,
	SPR_RIVER_RAFTS_FLAT_SE_NW			= 20823,
	SPR_RIVER_RAFTS_FLAT_FRONT_SW_NE	= 20824,
	SPR_RIVER_RAFTS_FLAT_FRONT_NW_SE	= 20825,
	SPR_RIVER_RAFTS_FLAT_FRONT_NE_SW	= 20826,
	SPR_RIVER_RAFTS_FLAT_FRONT_SE_NW	= 20827,
};

/** rct2: 0x0089B170 */
static void paint_river_rafts_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;

	if (direction & 1) {
		imageId = (direction == 1 ? SPR_RIVER_RAFTS_FLAT_NW_SE : SPR_RIVER_RAFTS_FLAT_SE_NW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 20, 32, 2, height, 6, 0, height, get_current_rotation());

		imageId = (direction == 1 ? SPR_RIVER_RAFTS_FLAT_FRONT_NW_SE : SPR_RIVER_RAFTS_FLAT_FRONT_SE_NW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 1, 32, 26, height, 27, 0, height, get_current_rotation());
	} else {
		imageId = (direction == 0 ? SPR_RIVER_RAFTS_FLAT_SW_NE : SPR_RIVER_RAFTS_FLAT_NE_SW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 20, 2, height, 0, 6, height, get_current_rotation());

		imageId = (direction == 0 ? SPR_RIVER_RAFTS_FLAT_FRONT_SW_NE : SPR_RIVER_RAFTS_FLAT_FRONT_NE_SW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 1, 26, height, 0, 27, height, get_current_rotation());
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

/**
 * rct2: 0x0089B0C0
 */
TRACK_PAINT_FUNCTION get_track_paint_function_river_rafts(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT:
		return paint_river_rafts_track_flat; break;
	case TRACK_ELEM_END_STATION:
		break;
	case TRACK_ELEM_BEGIN_STATION:
		break;
	case TRACK_ELEM_MIDDLE_STATION:
		break;
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
		break;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
		break;
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
		break;
	case TRACK_ELEM_S_BEND_LEFT:
	case TRACK_ELEM_S_BEND_RIGHT:
		break;
	}
	return NULL;
}
