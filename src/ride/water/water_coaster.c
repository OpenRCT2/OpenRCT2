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
	SPR_JUNIOR_RC_FLAT_SW_NE = 27807,
	SPR_JUNIOR_RC_FLAT_NW_SE = 27808,

	SPR_WATER_RC_FLAT_CHAIN_SW_NE = 27983,
	SPR_WATER_RC_FLAT_CHAIN_NW_SE = 27984,
};

static const uint32 water_rc_track_pieces_flat[2][4] = {
	{
		SPR_JUNIOR_RC_FLAT_SW_NE,
		SPR_JUNIOR_RC_FLAT_NW_SE,
		SPR_JUNIOR_RC_FLAT_SW_NE,
		SPR_JUNIOR_RC_FLAT_NW_SE
	},
	{
		SPR_WATER_RC_FLAT_CHAIN_SW_NE,
		SPR_WATER_RC_FLAT_CHAIN_NW_SE,
		SPR_WATER_RC_FLAT_CHAIN_SW_NE,
		SPR_WATER_RC_FLAT_CHAIN_NW_SE
	},
};

void paint_river_rafts_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement);

static void paint_water_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bool isChained = track_element_is_lift_hill(mapElement);
	uint32 imageId = water_rc_track_pieces_flat[isChained][direction] | gTrackColours[SCHEME_TRACK];
	sub_98196C_rotated(direction, imageId, 0, 6, 32, 20, 1, height);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);

	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup((direction & 1) ? 2 : 1, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_water_rc(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT:
		return paint_water_rc_track_flat;
	case TRACK_ELEM_FLAT_COVERED:
		return paint_river_rafts_track_flat;
	}
	return NULL;
}
