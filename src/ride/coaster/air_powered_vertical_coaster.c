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
#include "../../paint/supports.h"
#include "../../interface/viewport.h"
#include "../../paint/map_element/map_element.h"
#include "../../paint/paint.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../../world/sprite.h"
#include "../ride_data.h"
#include "../track_data.h"
#include "../track_paint.h"

enum {
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE = 22226,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE = 22227,
	SPR_AIR_POWERED_VERTICAL_RC_STATION_SW_NE = 22228,
	SPR_AIR_POWERED_VERTICAL_RC_STATION_NW_SE = 22229,
	SPR_AIR_POWERED_VERTICAL_RC_BRAKES_NW_SE = 22230,
	SPR_AIR_POWERED_VERTICAL_RC_BRAKES_SW_NE = 22231,

	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_SW_NE = 22334,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_NW_SE = 22335,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_NE_SW = 22336,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_SE_NW = 22337,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_SW_NE = 22338,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_NW_SE = 22339,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_NE_SW = 22340,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_SE_NW = 22341,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SW_NE = 22342,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE = 22343,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NE_SW = 22344,
	SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW = 22345,
	SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_SW_NE = 22346,
	SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_NW_SE = 22347,
	SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_NE_SW = 22348,
	SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_SE_NW = 22349,
};

static void air_powered_vertical_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4] = {
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE },
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE },
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE },
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE },
	};

	uint32 imageId = imageIds[direction] | gTrackColours[SCHEME_TRACK];
	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height);

	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][2] = {
		{ SPR_AIR_POWERED_VERTICAL_RC_STATION_SW_NE, SPR_STATION_BASE_B_SW_NE },
		{ SPR_AIR_POWERED_VERTICAL_RC_STATION_NW_SE, SPR_STATION_BASE_B_NW_SE },
		{ SPR_AIR_POWERED_VERTICAL_RC_STATION_SW_NE, SPR_STATION_BASE_B_SW_NE },
		{ SPR_AIR_POWERED_VERTICAL_RC_STATION_NW_SE, SPR_STATION_BASE_B_NW_SE },
	};

	sub_98197C_rotated(direction, imageIds[direction][1] | gTrackColours[SCHEME_MISC], 0, 0, 32, 28, 1, height - 2, 0, 2, height);
	sub_98199C_rotated(direction, imageIds[direction][0] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 6, height);

	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	rct_ride * ride = get_ride(rideIndex);
	track_paint_util_draw_station_platform(ride, direction, height, 5, mapElement);

	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][2] = {
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_SW_NE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SW_NE },
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_NW_SE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE },
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_NE_SW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NE_SW },
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_SE_NW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW },
	};

	uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);

	if (direction == 0 || direction == 1) {
		imageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];
		sub_98197C_rotated(direction, imageId, 0, 0, 32, 1, 26, height, 0, 27, height);
	}

	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][2] = {
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_SW_NE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SW_NE },
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_NW_SE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE },
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_NE_SW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NE_SW },
		{ SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_SE_NW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW },
	};

	uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);

	if (direction == 2 || direction == 3) {
		imageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];
		sub_98197C_rotated(direction, imageId, 0, 0, 32, 1, 26, height, 0, 27, height);
	}

	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	air_powered_vertical_rc_track_flat_to_right_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void air_powered_vertical_rc_track_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	air_powered_vertical_rc_track_flat_to_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void air_powered_vertical_rc_track_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4] = {
		{ SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_SW_NE },
		{ SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_NW_SE },
		{ SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_NE_SW },
		{ SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_SE_NW },
	};

	uint32 imageId = imageIds[direction] | gTrackColours[SCHEME_TRACK];
	if (direction == 0 || direction == 1) {
		sub_98197C_rotated(direction, imageId, 0, 0, 32, 1, 26, height, 0, 27, height);
	} else {
		sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);
	}

	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	air_powered_vertical_rc_track_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void air_powered_vertical_rc_track_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4] = {
		{ SPR_AIR_POWERED_VERTICAL_RC_BRAKES_NW_SE },
		{ SPR_AIR_POWERED_VERTICAL_RC_BRAKES_SW_NE },
		{ SPR_AIR_POWERED_VERTICAL_RC_BRAKES_NW_SE },
		{ SPR_AIR_POWERED_VERTICAL_RC_BRAKES_SW_NE },
	};

	uint32 imageId = imageIds[direction] | gTrackColours[SCHEME_TRACK];
	sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height);

	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_air_powered_vertical_rc(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT:
		return air_powered_vertical_rc_track_flat;
	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
		return air_powered_vertical_rc_track_station;

	case TRACK_ELEM_FLAT_TO_LEFT_BANK:
		return air_powered_vertical_rc_track_flat_to_left_bank;
	case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
		return air_powered_vertical_rc_track_flat_to_right_bank;
	case TRACK_ELEM_LEFT_BANK_TO_FLAT:
		return air_powered_vertical_rc_track_left_bank_to_flat;
	case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
		return air_powered_vertical_rc_track_right_bank_to_flat;
	case TRACK_ELEM_LEFT_BANK:
		return air_powered_vertical_rc_track_left_bank;
	case TRACK_ELEM_RIGHT_BANK:
		return air_powered_vertical_rc_track_right_bank;

	case TRACK_ELEM_BRAKES:
		return air_powered_vertical_rc_track_brakes;
	}
	return NULL;
}
