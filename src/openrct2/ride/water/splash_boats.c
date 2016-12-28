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

#include "../../config.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../../world/sprite.h"
#include "../track.h"
#include "../track_paint.h"
#include "../vehicle_paint.h"

enum {
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SW_NE = 20828,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NW_SE = 20829,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NE_SW = 20830,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SE_NW = 20831,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 20832,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 20833,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 20834,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 20835,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SW_NE = 20836,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NW_SE = 20837,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NE_SW = 20838,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SE_NW = 20839,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 20840,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 20841,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 20842,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 20843,
	SPR_SPLASH_BOATS_25_DEG_UP_SW_NE = 20844,
	SPR_SPLASH_BOATS_25_DEG_UP_NW_SE = 20845,
	SPR_SPLASH_BOATS_25_DEG_UP_NE_SW = 20846,
	SPR_SPLASH_BOATS_25_DEG_UP_SE_NW = 20847,
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SW_NE = 20848,
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NW_SE = 20849,
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NE_SW = 20850,
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SE_NW = 20851,
	SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NE_SW = 20852,
	SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SE_NW = 20853,
	SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SW_NE = 20854,
	SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NW_SE = 20855,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NE_SW = 20856,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SE_NW = 20857,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SW_NE = 20858,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NW_SE = 20859,
	SPR_SPLASH_BOATS_25_DEG_DOWN_NE_SW = 20860,
	SPR_SPLASH_BOATS_25_DEG_DOWN_SE_NW = 20861,
	SPR_SPLASH_BOATS_25_DEG_DOWN_SW_NE = 20862,
	SPR_SPLASH_BOATS_25_DEG_DOWN_NW_SE = 20863,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SW_NE = 20864,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NW_SE = 20865,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NE_SW = 20866,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SE_NW = 20867,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SW_NE = 20868,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NW_SE = 20869,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NE_SW = 20870,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SE_NW = 20871,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SW_NE = 20872,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NW_SE = 20873,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NE_SW = 20874,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SE_NW = 20875,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SW_NE = 20876,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NW_SE = 20877,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NE_SW = 20878,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SE_NW = 20879,
	SPR_SPLASH_BOATS_60_DEG_UP_SW_NE = 20880,
	SPR_SPLASH_BOATS_60_DEG_UP_NW_SE = 20881,
	SPR_SPLASH_BOATS_60_DEG_UP_NE_SW = 20882,
	SPR_SPLASH_BOATS_60_DEG_UP_SE_NW = 20883,
	SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SW_NE = 20884,
	SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NW_SE = 20885,
	SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NE_SW = 20886,
	SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SE_NW = 20887,
};

uint32 SplashBoats25DegUpImageId[4] =
{
	SPR_SPLASH_BOATS_25_DEG_UP_SW_NE,
	SPR_SPLASH_BOATS_25_DEG_UP_NW_SE,
	SPR_SPLASH_BOATS_25_DEG_UP_NE_SW,
	SPR_SPLASH_BOATS_25_DEG_UP_SE_NW,
};

uint32 SplashBoats25DegUpFrontImageId[4] =
{
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SW_NE,
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NW_SE,
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NE_SW,
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SE_NW,
};

uint32 SplashBoats60DegUpImageId[4] =
{
	SPR_SPLASH_BOATS_60_DEG_UP_SW_NE,
	SPR_SPLASH_BOATS_60_DEG_UP_NW_SE,
	SPR_SPLASH_BOATS_60_DEG_UP_NE_SW,
	SPR_SPLASH_BOATS_60_DEG_UP_SE_NW,
};

uint32 SplashBoats60DegUpFrontImageId[4] =
{
	SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SW_NE,
	SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NW_SE,
	SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NE_SW,
	SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SE_NW,
};

uint32 SplashBoatsFlatTo25DegUpImageId[4] =
{
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SW_NE,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NW_SE,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NE_SW,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SE_NW,
};

uint32 SplashBoatsFlatTo25DegUpFrontImageId[4] =
{
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SW_NE,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NW_SE,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NE_SW,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SE_NW,
};

uint32 SplashBoats25DegUpToFlatImageId[4] =
{
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SW_NE,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NW_SE,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NE_SW,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SE_NW,
};

uint32 SplashBoats25DegUpToFlatFrontImageId[4] =
{
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SW_NE,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NW_SE,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NE_SW,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SE_NW,
};

uint32 SplashBoats25DegUpTo60DegUpImageId[4] =
{
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SW_NE,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NW_SE,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NE_SW,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SE_NW,
};

uint32 SplashBoats25DegUpTo60DegUpFrontImageId[4] =
{
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SW_NE,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NW_SE,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NE_SW,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SE_NW,
};

uint32 SplashBoats60DegUpTo25DegUpImageId[4] =
{
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SW_NE,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NW_SE,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NE_SW,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SE_NW,
};

uint32 SplashBoats60DegUpTo25DegUpFrontImageId[4] =
{
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SW_NE,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NW_SE,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NE_SW,
	SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SE_NW,
};

uint32 SplashBoats25DegDownImageId[4] =
{
	SPR_SPLASH_BOATS_25_DEG_DOWN_SW_NE,
	SPR_SPLASH_BOATS_25_DEG_DOWN_NW_SE,
	SPR_SPLASH_BOATS_25_DEG_DOWN_NE_SW,
	SPR_SPLASH_BOATS_25_DEG_DOWN_SE_NW,
};

uint32 SplashBoats25DegDownFrontImageId[4] =
{
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NE_SW,
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SE_NW,
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SW_NE,
	SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NW_SE,
};

uint32 SplashBoatsFlatTo25DegDownImageId[4] =
{
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SW_NE,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NW_SE,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NE_SW,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SE_NW,
};

uint32 SplashBoatsFlatTo25DegDownFrontImageId[4] =
{
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NE_SW,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SE_NW,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SW_NE,
	SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NW_SE,
};

uint32 SplashBoats25DegDownToFlatImageId[4] =
{
	SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SW_NE,
	SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NW_SE,
	SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NE_SW,
	SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SE_NW,
};

uint32 SplashBoats25DegDownToFlatFrontImageId[4] =
{
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NE_SW,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SE_NW,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SW_NE,
	SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NW_SE,
};

static void paint_splash_boats_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	uint32 imageId = SplashBoats25DegUpImageId[direction] | gTrackColours[SCHEME_TRACK];
	uint32 frontImageId = SplashBoats25DegUpFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
	sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 50, height, 0, 27, height);

	wooden_a_supports_paint_setup((direction & 1), 9 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void paint_splash_boats_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	uint32 imageId = SplashBoats60DegUpImageId[direction] | gTrackColours[SCHEME_TRACK];
	uint32 frontImageId = SplashBoats60DegUpFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

	gWoodenSupportsPrependTo = sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
	sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 98, height, 0, 27, height);

	wooden_a_supports_paint_setup((direction & 1), 21 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 56, TUNNEL_8);
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 104, 0x20);
}

static void paint_splash_boats_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	uint32 imageId = SplashBoatsFlatTo25DegUpImageId[direction] | gTrackColours[SCHEME_TRACK];
	uint32 frontImageId = SplashBoatsFlatTo25DegUpFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
	sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 42, height, 0, 27, height);

	wooden_a_supports_paint_setup((direction & 1), 1 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void paint_splash_boats_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	uint32 imageId = SplashBoats25DegUpToFlatImageId[direction] | gTrackColours[SCHEME_TRACK];
	uint32 frontImageId = SplashBoats25DegUpToFlatFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
	sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 34, height, 0, 27, height);

	wooden_a_supports_paint_setup((direction & 1), 5 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void paint_splash_boats_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	uint32 imageId = SplashBoats25DegUpTo60DegUpImageId[direction] | gTrackColours[SCHEME_TRACK];
	uint32 frontImageId = SplashBoats25DegUpTo60DegUpFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

	gWoodenSupportsPrependTo = sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
	sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 66, height, 0, 27, height);

	wooden_a_supports_paint_setup((direction & 1), 13 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_8);
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 72, 0x20);
}

static void paint_splash_boats_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	uint32 imageId = SplashBoats60DegUpTo25DegUpImageId[direction] | gTrackColours[SCHEME_TRACK];
	uint32 frontImageId = SplashBoats60DegUpTo25DegUpFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

	gWoodenSupportsPrependTo = sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
	sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 66, height, 0, 27, height);

	wooden_a_supports_paint_setup((direction & 1), 17 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_8);
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 72, 0x20);
}

static void paint_splash_boats_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	uint32 imageId = SplashBoats25DegDownImageId[direction] | gTrackColours[SCHEME_TRACK];
	uint32 frontImageId = SplashBoats25DegDownFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
	sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 50, height, 0, 27, height);

	static const uint8 specialSupport[] = { 11, 12, 9, 10 };
	wooden_a_supports_paint_setup((direction & 1), specialSupport[direction], height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
	} else {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void paint_splash_boats_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	paint_splash_boats_track_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void paint_splash_boats_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	uint32 imageId = SplashBoatsFlatTo25DegDownImageId[direction] | gTrackColours[SCHEME_TRACK];
	uint32 frontImageId = SplashBoatsFlatTo25DegDownFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
	sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 34, height, 0, 27, height);

	static const uint8 specialSupport[] = { 7, 8, 5, 6 };
	wooden_a_supports_paint_setup((direction & 1), specialSupport[direction], height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
	} else {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void paint_splash_boats_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	paint_splash_boats_track_60_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void paint_splash_boats_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	paint_splash_boats_track_25_deg_up_to_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void paint_splash_boats_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element *mapElement)
{
	uint32 imageId = SplashBoats25DegDownToFlatImageId[direction] | gTrackColours[SCHEME_TRACK];
	uint32 frontImageId = SplashBoats25DegDownToFlatFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
	sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 42, height, 0, 27, height);

	static const uint8 specialSupport[] = { 3, 4, 1, 2 };
	wooden_a_supports_paint_setup((direction & 1), specialSupport[direction], height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_8);
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_splash_boats(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_25_DEG_UP:
		return paint_splash_boats_track_25_deg_up;
	case TRACK_ELEM_60_DEG_UP:
		return paint_splash_boats_track_60_deg_up;
	case TRACK_ELEM_FLAT_TO_25_DEG_UP:
		return paint_splash_boats_track_flat_to_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
		return paint_splash_boats_track_25_deg_up_to_60_deg_up;
	case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
		return paint_splash_boats_track_60_deg_up_to_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_FLAT:
		return paint_splash_boats_track_25_deg_up_to_flat;
	case TRACK_ELEM_25_DEG_DOWN:
		return paint_splash_boats_track_25_deg_down;
	case TRACK_ELEM_60_DEG_DOWN:
		return paint_splash_boats_track_60_deg_down;
	case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
		return paint_splash_boats_track_flat_to_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
		return paint_splash_boats_track_25_deg_down_to_60_deg_down;
	case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
		return paint_splash_boats_track_60_deg_down_to_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
		return paint_splash_boats_track_25_deg_down_to_flat;

	//////////////////////////////////////////////////////
	// Use River Rafts
	//////////////////////////////////////////////////////
	case TRACK_ELEM_FLAT:
	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
	case TRACK_ELEM_S_BEND_LEFT:
	case TRACK_ELEM_S_BEND_RIGHT:
		return get_track_paint_function_river_rafts(trackType, direction);
	}

	return NULL;
}

#ifndef NO_VEHICLES
/**
 *
 *  rct2: 0x006D4295
 */
void vehicle_visual_splash_boats_or_water_coaster(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	if (vehicle->is_child) {
		vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
	} else {
		vehicle = GET_VEHICLE(vehicle->next_vehicle_on_ride);
	}
	g_currently_drawn_item = vehicle;
	imageDirection = ((get_current_rotation() * 8) + vehicle->sprite_direction) & 0x1F;
	gUnk9DE568 = vehicle->x;
	gUnk9DE56C = vehicle->y;
	vehicle_paint(vehicle, imageDirection);
}
#endif
