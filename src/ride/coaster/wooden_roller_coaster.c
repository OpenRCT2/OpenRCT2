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
	SPR_WOODEN_RC_25_DEG_FRONT_NW_SE = 23569,
	SPR_WOODEN_RC_25_DEG_FRONT_NE_SW = 23570,

	SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NW_SE = 23603,
	SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NE_SW = 23604,

	SPR_WOODEN_RC_FLAT_CHAIN_SW_NE = 23749,
	SPR_WOODEN_RC_FLAT_CHAIN_NW_SE = 23750,
	SPR_WOODEN_RC_FLAT_CHAIN_NE_SW = 23751,
	SPR_WOODEN_RC_FLAT_CHAIN_SE_NW = 23752,
	SPR_WOODEN_RC_FLAT_SW_NE = 23753,
	SPR_WOODEN_RC_FLAT_NW_SE = 23754,
	SPR_WOODEN_RC_BRAKES_SW_NE = 23755,
	SPR_WOODEN_RC_BRAKES_NW_SE = 23756,
	SPR_WOODEN_RC_BLOCK_BRAKES_SW_NE = 23757,
	SPR_WOODEN_RC_BLOCK_BRAKES_NW_SE = 23758,

	SPR_WOODEN_RC_25_DEG_SW_NE = 23545,
	SPR_WOODEN_RC_25_DEG_NW_SE = 23546,
	SPR_WOODEN_RC_25_DEG_NE_SW = 23547,
	SPR_WOODEN_RC_25_DEG_SE_NW = 23548,

	SPR_WOODEN_RC_25_DEG_CHAIN_SW_NE = 23579,
	SPR_WOODEN_RC_25_DEG_CHAIN_NW_SE = 23580,
	SPR_WOODEN_RC_25_DEG_CHAIN_NE_SW = 23581,
	SPR_WOODEN_RC_25_DEG_CHAIN_SE_NW = 23582,

	SPR_WOODEN_RC_STATION_SW_NE = 23973,
	SPR_WOODEN_RC_STATION_NW_SE = 23974,

	SPR_WOODEN_RC_25_DEG_RAILS_SW_NE = 24411,
	SPR_WOODEN_RC_25_DEG_RAILS_NW_SE = 24412,
	SPR_WOODEN_RC_25_DEG_RAILS_NE_SW = 24413,
	SPR_WOODEN_RC_25_DEG_RAILS_SE_NW = 24414,

	SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NW_SE = 24435,
	SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NE_SW = 24436,

	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SW_NE = 24445,
	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NW_SE = 24446,
	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NE_SW = 24447,
	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SE_NW = 24448,

	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NW_SE = 24469,
	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NE_SW = 24470,

	SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SW_NE = 24615,
	SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NW_SE = 24616,
	SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NE_SW = 24617,
	SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SE_NW = 24618,
	SPR_WOODEN_RC_FLAT_RAILS_SW_NE = 24619,
	SPR_WOODEN_RC_FLAT_RAILS_NW_SE = 24620,
	SPR_WOODEN_RC_BRAKES_RAILS_SW_NE = 24621,
	SPR_WOODEN_RC_BRAKES_RAILS_NW_SE = 24622,
	SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_SW_NE = 24623,
	SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_NW_SE = 24624,

	SPR_WOODEN_RC_STATION_RAILS_SW_NE = 24839,
	SPR_WOODEN_RC_STATION_RAILS_NW_SE = 24840,
};

static const uint32 _wooden_rc_block_brakes_image_ids[4][2] = {
	{ SPR_WOODEN_RC_BLOCK_BRAKES_SW_NE, SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_SW_NE },
	{ SPR_WOODEN_RC_BLOCK_BRAKES_NW_SE, SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_NW_SE },
	{ SPR_WOODEN_RC_BLOCK_BRAKES_SW_NE, SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_SW_NE },
	{ SPR_WOODEN_RC_BLOCK_BRAKES_NW_SE, SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_NW_SE },
};

static uint32 wooden_rc_get_track_colour()
{
	return (gTrackColours[SCHEME_TRACK] & ~0xF80000) | gTrackColours[SCHEME_SUPPORTS];
}

static uint32 wooden_rc_get_rails_colour()
{
	return gTrackColours[SCHEME_TRACK];
}

static void wooden_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][2] = {
		{ SPR_WOODEN_RC_FLAT_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_SW_NE },
		{ SPR_WOODEN_RC_FLAT_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_NW_SE },
		{ SPR_WOODEN_RC_FLAT_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_NE_SW },
		{ SPR_WOODEN_RC_FLAT_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_SE_NW },
	};
	static const uint32 railsImageIds[4][2] = {
		{ SPR_WOODEN_RC_FLAT_RAILS_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SW_NE },
		{ SPR_WOODEN_RC_FLAT_RAILS_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NW_SE },
		{ SPR_WOODEN_RC_FLAT_RAILS_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NE_SW },
		{ SPR_WOODEN_RC_FLAT_RAILS_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SE_NW },
	};

	uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
	uint32 imageId = imageIds[direction][isChained] | wooden_rc_get_track_colour();
	uint32 railsImageId = railsImageIds[direction][isChained] | wooden_rc_get_rails_colour();
	sub_98197C_rotated(direction, imageId, 0, 2, 32, 25, 2, height, 0, 3, height);
	sub_98199C_rotated(direction, railsImageId, 0, 2, 32, 25, 2, height, 0, 3, height);
	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void wooden_rc_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 stationImageIds[4][2] = {
		{ SPR_WOODEN_RC_STATION_SW_NE, SPR_WOODEN_RC_STATION_RAILS_SW_NE },
		{ SPR_WOODEN_RC_STATION_NW_SE, SPR_WOODEN_RC_STATION_RAILS_NW_SE },
		{ SPR_WOODEN_RC_STATION_SW_NE, SPR_WOODEN_RC_STATION_RAILS_SW_NE },
		{ SPR_WOODEN_RC_STATION_NW_SE, SPR_WOODEN_RC_STATION_RAILS_NW_SE },
	};

	int trackType = mapElement->properties.track.type;
	if (trackType == TRACK_ELEM_END_STATION) {
		uint32 imageId = _wooden_rc_block_brakes_image_ids[direction][0] | wooden_rc_get_track_colour();
		uint32 railsImageId = _wooden_rc_block_brakes_image_ids[direction][1] | wooden_rc_get_rails_colour();
		sub_98197C_rotated(direction, imageId | gTrackColours[SCHEME_TRACK], 0, 2, 32, 27, 2, height, 0, 2, height);
		sub_98199C_rotated(direction, railsImageId | gTrackColours[SCHEME_TRACK], 0, 2, 32, 27, 2, height, 0, 2, height);
	} else {
		uint32 imageId = stationImageIds[direction][0] | wooden_rc_get_track_colour();
		uint32 railsImageId = stationImageIds[direction][1] | wooden_rc_get_rails_colour();
		sub_98197C_rotated(direction, imageId | gTrackColours[SCHEME_TRACK], 0, 2, 32, 27, 2, height, 0, 2, height);
		sub_98199C_rotated(direction, railsImageId | gTrackColours[SCHEME_TRACK], 0, 2, 32, 27, 2, height, 0, 2, height);
	}
	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
	track_paint_util_draw_station(rideIndex, trackSequence, direction, height + 4, mapElement);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void wooden_rc_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[2][4][4] = {
		{
			{ SPR_WOODEN_RC_25_DEG_SW_NE, SPR_WOODEN_RC_25_DEG_RAILS_SW_NE, 0 },
			{ SPR_WOODEN_RC_25_DEG_NW_SE, SPR_WOODEN_RC_25_DEG_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NW_SE },
			{ SPR_WOODEN_RC_25_DEG_NE_SW, SPR_WOODEN_RC_25_DEG_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NE_SW },
			{ SPR_WOODEN_RC_25_DEG_SE_NW, SPR_WOODEN_RC_25_DEG_RAILS_SE_NW, 0 },
		},
		{
			{ SPR_WOODEN_RC_25_DEG_CHAIN_SW_NE, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SW_NE, 0 },
			{ SPR_WOODEN_RC_25_DEG_CHAIN_NW_SE, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NW_SE },
			{ SPR_WOODEN_RC_25_DEG_CHAIN_NE_SW, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NE_SW },
			{ SPR_WOODEN_RC_25_DEG_CHAIN_SE_NW, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SE_NW, 0 },
		}
	};

	uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
	uint32 imageId = imageIds[isChained][direction][0] | wooden_rc_get_track_colour();
	uint32 railsImageId = imageIds[isChained][direction][1] | wooden_rc_get_rails_colour();
	sub_98197C_rotated(direction, imageId, 0, 0, 32, 25, 2, height, 0, 3, height);
	sub_98199C_rotated(direction, railsImageId, 0, 0, 32, 25, 2, height, 0, 3, height);
	if (direction == 1 || direction == 2) {
		imageId = imageIds[isChained][direction][2] | wooden_rc_get_track_colour();
		railsImageId = imageIds[isChained][direction][3] | wooden_rc_get_rails_colour();
		sub_98197C_rotated(direction, imageId, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
		sub_98199C_rotated(direction, railsImageId, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
	}
	wooden_a_supports_paint_setup(direction & 1, 9 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void wooden_rc_track_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][2] = {
		{ SPR_WOODEN_RC_BRAKES_SW_NE, SPR_WOODEN_RC_BRAKES_RAILS_SW_NE },
		{ SPR_WOODEN_RC_BRAKES_NW_SE, SPR_WOODEN_RC_BRAKES_RAILS_NW_SE },
		{ SPR_WOODEN_RC_BRAKES_SW_NE, SPR_WOODEN_RC_BRAKES_RAILS_SW_NE },
		{ SPR_WOODEN_RC_BRAKES_NW_SE, SPR_WOODEN_RC_BRAKES_RAILS_NW_SE },
	};

	uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
	uint32 imageId = imageIds[direction][0] | wooden_rc_get_track_colour();
	uint32 railsImageId = imageIds[direction][1] | wooden_rc_get_rails_colour();
	sub_98197C_rotated(direction, imageId, 0, 2, 32, 25, 2, height, 0, 3, height);
	sub_98199C_rotated(direction, railsImageId, 0, 2, 32, 25, 2, height, 0, 3, height);
	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void wooden_rc_track_block_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
	uint32 imageId = _wooden_rc_block_brakes_image_ids[direction][0] | wooden_rc_get_track_colour();
	uint32 railsImageId = _wooden_rc_block_brakes_image_ids[direction][1] | wooden_rc_get_rails_colour();
	sub_98197C_rotated(direction, imageId, 0, 2, 32, 25, 2, height, 0, 3, height);
	sub_98199C_rotated(direction, railsImageId, 0, 2, 32, 25, 2, height, 0, 3, height);
	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_wooden_rc(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT:
		return wooden_rc_track_flat;
	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
		return wooden_rc_track_station;
	case TRACK_ELEM_25_DEG_UP:
		return wooden_rc_track_25_deg_up;
	case TRACK_ELEM_BRAKES:
		return wooden_rc_track_brakes;
	case TRACK_ELEM_BLOCK_BRAKES:
		return wooden_rc_track_block_brakes;
	}
	return NULL;
}
