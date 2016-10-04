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

#include "../../drawing/drawing.h"
#include "../../interface/viewport.h"
#include "../../paint/map_element/map_element.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../../world/sprite.h"
#include "../ride_data.h"
#include "../track_data.h"
#include "../track_paint.h"

enum {
	SPR_WILD_MOUSE_FLAT_SW_NE = 16900,
	SPR_WILD_MOUSE_FLAT_NW_SE = 16901,
	SPR_WILD_MOUSE_STATION_SW_NE = 16902,
	SPR_WILD_MOUSE_STATION_NW_SE = 16903,
	SPR_WILD_MOUSE_STATION_END_SW_NE = 16904,
	SPR_WILD_MOUSE_STATION_END_NW_SE = 16905,
};

static void wild_mouse_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4] = {
		SPR_WILD_MOUSE_FLAT_SW_NE,
		SPR_WILD_MOUSE_FLAT_NW_SE,
		SPR_WILD_MOUSE_FLAT_SW_NE,
		SPR_WILD_MOUSE_FLAT_NW_SE,
	};

	uint32 imageId = imageIds[direction] | gTrackColours[SCHEME_TRACK];
	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, -1, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void wild_mouse_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][3] = {
		{ SPR_WILD_MOUSE_STATION_SW_NE, SPR_WILD_MOUSE_STATION_END_SW_NE, SPR_STATION_BASE_B_SW_NE },
		{ SPR_WILD_MOUSE_STATION_NW_SE, SPR_WILD_MOUSE_STATION_END_NW_SE, SPR_STATION_BASE_B_NW_SE },
		{ SPR_WILD_MOUSE_STATION_SW_NE, SPR_WILD_MOUSE_STATION_END_SW_NE, SPR_STATION_BASE_B_SW_NE },
		{ SPR_WILD_MOUSE_STATION_NW_SE, SPR_WILD_MOUSE_STATION_END_NW_SE, SPR_STATION_BASE_B_NW_SE },
	};

	int trackType = mapElement->properties.track.type;
	int t = (trackType == TRACK_ELEM_END_STATION ? 1 : 0);
	sub_98197C_rotated(direction, imageIds[direction][2] | gTrackColours[SCHEME_MISC], 0, 0, 32, 28, 2, height - 2, 0, 2, height);
	sub_98199C_rotated(direction, imageIds[direction][t] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 2, height, 0, 0, height);
	track_paint_util_draw_station_metal_supports(direction, height, gTrackColours[SCHEME_SUPPORTS]);
	track_paint_util_draw_station(rideIndex, trackSequence, direction, height, mapElement);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_wild_mouse(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT:
		return wild_mouse_track_flat;
	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
		return wild_mouse_track_station;
	}
	return NULL;
}
