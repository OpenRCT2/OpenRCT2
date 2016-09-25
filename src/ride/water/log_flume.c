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
#include "../../world/sprite.h"
#include "../track.h"
#include "../track_paint.h"

enum {
	SPR_LOG_FLUME_FLAT_SW_NE = 20996,
	SPR_LOG_FLUME_FLAT_NW_SE = 20997,
	SPR_LOG_FLUME_FLAT_NE_SW = 20998,
	SPR_LOG_FLUME_FLAT_SE_NW = 20999,
	SPR_LOG_FLUME_FLAT_FRONT_SW_NE = 21000,
	SPR_LOG_FLUME_FLAT_FRONT_NW_SE = 21001,
	SPR_LOG_FLUME_FLAT_FRONT_NE_SW = 21002,
	SPR_LOG_FLUME_FLAT_FRONT_SE_NW = 21003,
};

static const uint32 LogFlumeTrackFlatImageIds[4][2] =
{
	{ SPR_LOG_FLUME_FLAT_SW_NE, SPR_LOG_FLUME_FLAT_FRONT_SW_NE },
	{ SPR_LOG_FLUME_FLAT_NW_SE, SPR_LOG_FLUME_FLAT_FRONT_NW_SE },
	{ SPR_LOG_FLUME_FLAT_NE_SW, SPR_LOG_FLUME_FLAT_FRONT_NE_SW },
	{ SPR_LOG_FLUME_FLAT_SE_NW, SPR_LOG_FLUME_FLAT_FRONT_SE_NW },
};

static void paint_log_flume_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId = LogFlumeTrackFlatImageIds[direction][0] | gTrackColours[SCHEME_TRACK];
	uint32 frontImageId = LogFlumeTrackFlatImageIds[direction][1] | gTrackColours[SCHEME_TRACK];

	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
	sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 26, height, 0, 27, height);

	metal_a_supports_paint_setup(3, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);

	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void paint_log_flume_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_ride *ride = get_ride(rideIndex);

	uint32 imageId = LogFlumeTrackFlatImageIds[direction][0] | gTrackColours[SCHEME_TRACK];
	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height + 3);

	if (direction & 1) {
		imageId = SPR_STATION_BASE_B_NW_SE | gTrackColours[SCHEME_MISC];
	} else {
		imageId = SPR_STATION_BASE_B_SW_NE | gTrackColours[SCHEME_MISC];
	}
	sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());

	if (direction & 1) {
		metal_a_supports_paint_setup(3, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		metal_a_supports_paint_setup(3, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	} else {
		metal_a_supports_paint_setup(3, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		metal_a_supports_paint_setup(3, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	track_paint_util_draw_station(rideIndex, trackSequence, direction, height + 2, mapElement);

	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_log_flume(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT:
		return paint_log_flume_track_flat;

	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
		return paint_log_flume_track_station;
	}

	return NULL;
}
