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
	SPR_WOODEN_RC_FLAT_TO_25_DEG_SW_NE = 23537,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_NW_SE = 23538,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_NE_SW = 23539,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_SE_NW = 23540,

	SPR_WOODEN_RC_25_DEG_SW_NE = 23545,
	SPR_WOODEN_RC_25_DEG_NW_SE = 23546,
	SPR_WOODEN_RC_25_DEG_NE_SW = 23547,
	SPR_WOODEN_RC_25_DEG_SE_NW = 23548,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_SW_NE = 23549,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_FRONT_NW_SE = 23550,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_NW_SE = 23551,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_FRONT_NE_SW = 23552,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_NE_SW = 23553,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_SE_NW = 23554,

	SPR_WOODEN_RC_60_DEG_SW_NE = 23561,

	SPR_WOODEN_RC_60_DEG_CHAIN_SW_NE = 23561,
	SPR_WOODEN_RC_60_DEG_NW_SE = 23562,

	SPR_WOODEN_RC_60_DEG_CHAIN_NW_SE = 23562,
	SPR_WOODEN_RC_60_DEG_NE_SW = 23563,

	SPR_WOODEN_RC_60_DEG_CHAIN_NE_SW = 23563,
	SPR_WOODEN_RC_60_DEG_SE_NW = 23564,

	SPR_WOODEN_RC_60_DEG_CHAIN_SE_NW = 23564,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_FRONT_NW_SE = 23565,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_FRONT_NE_SW = 23566,

	SPR_WOODEN_RC_25_DEG_FRONT_NW_SE = 23569,

	SPR_WOODEN_RC_60_DEG_FRONT_NW_SE = 23569,
	SPR_WOODEN_RC_25_DEG_FRONT_NE_SW = 23570,

	SPR_WOODEN_RC_60_DEG_FRONT_NE_SW = 23570,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_SW_NE = 23571,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_NW_SE = 23572,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_NE_SW = 23573,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_SE_NW = 23574,

	SPR_WOODEN_RC_25_DEG_CHAIN_SW_NE = 23579,
	SPR_WOODEN_RC_25_DEG_CHAIN_NW_SE = 23580,
	SPR_WOODEN_RC_25_DEG_CHAIN_NE_SW = 23581,
	SPR_WOODEN_RC_25_DEG_CHAIN_SE_NW = 23582,

	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_FRONT_NW_SE = 23599,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_FRONT_NE_SW = 23600,

	SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NW_SE = 23603,

	SPR_WOODEN_RC_60_DEG_CHAIN_FRONT_NW_SE = 23603,
	SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NE_SW = 23604,

	SPR_WOODEN_RC_60_DEG_CHAIN_FRONT_NE_SW = 23604,

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

	SPR_WOODEN_RC_STATION_SW_NE = 23973,
	SPR_WOODEN_RC_STATION_NW_SE = 23974,

	SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_SW_NE = 24403,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_NW_SE = 24404,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_NE_SW = 24405,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_SE_NW = 24406,

	SPR_WOODEN_RC_25_DEG_RAILS_SW_NE = 24411,
	SPR_WOODEN_RC_25_DEG_RAILS_NW_SE = 24412,
	SPR_WOODEN_RC_25_DEG_RAILS_NE_SW = 24413,
	SPR_WOODEN_RC_25_DEG_RAILS_SE_NW = 24414,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_SW_NE = 24415,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_FRONT_NW_SE = 24416,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_NW_SE = 24417,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_FRONT_NE_SW = 24418,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_NE_SW = 24419,
	SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_SE_NW = 24420,

	SPR_WOODEN_RC_60_DEG_RAILS_SW_NE = 24427,

	SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_SW_NE = 24427,
	SPR_WOODEN_RC_60_DEG_RAILS_NW_SE = 24428,

	SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_NW_SE = 24428,
	SPR_WOODEN_RC_60_DEG_RAILS_NE_SW = 24429,

	SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_NE_SW = 24429,
	SPR_WOODEN_RC_60_DEG_RAILS_SE_NW = 24430,

	SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_SE_NW = 24430,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_FRONT_NW_SE = 24431,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_FRONT_NE_SW = 24432,

	SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NW_SE = 24435,

	SPR_WOODEN_RC_60_DEG_RAILS_FRONT_NW_SE = 24435,
	SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NE_SW = 24436,

	SPR_WOODEN_RC_60_DEG_RAILS_FRONT_NE_SW = 24436,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_SW_NE = 24437,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_NW_SE = 24438,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_NE_SW = 24439,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_SE_NW = 24440,

	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SW_NE = 24445,
	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NW_SE = 24446,
	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NE_SW = 24447,
	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SE_NW = 24448,

	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_FRONT_NW_SE = 24465,
	SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_FRONT_NE_SW = 24466,

	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NW_SE = 24469,

	SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_FRONT_NW_SE = 24469,
	SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NE_SW = 24470,

	SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_FRONT_NE_SW = 24470,

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

	SPR_WOODEN_RC_60_DEG_TO_25_DEG_SW_NE = 23555,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_NW_SE = 23557,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_NE_SW = 23559,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_SE_NW = 23560,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_SW_NE = 24421,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_NW_SE = 24423,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_NE_SW = 24425,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_SE_NW = 24426,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_FRONT_NW_SE = 23556,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_FRONT_NE_SW = 23558,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_FRONT_NW_SE = 24422,
	SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_FRONT_NE_SW = 24424,

	SPR_WOODEN_RC_25_DEG_TO_FLAT_SW_NE = 23541,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_NW_SE = 23542,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_NE_SW = 23543,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_SE_NW = 23544,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_SW_NE = 24407,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_NW_SE = 24408,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_NE_SW = 24409,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_SE_NW = 24410,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_FRONT_NW_SE = 23567,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_FRONT_NE_SW = 23568,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_FRONT_NW_SE = 24433,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_FRONT_NE_SW = 24434,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_SW_NE = 23575,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_NW_SE = 23576,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_NE_SW = 23577,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_SE_NW = 23578,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_SW_NE = 24441,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_NW_SE = 24442,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_NE_SW = 24443,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_SE_NW = 24444,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_FRONT_NW_SE = 23601,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_FRONT_NE_SW = 23602,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_FRONT_NW_SE = 24467,
	SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_FRONT_NE_SW = 24468,
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

static void wooden_rc_track_paint(
	uint32 imageIdTrack,
	uint32 imageIdRails,
	uint8 direction,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	sint16 z_offset,
	sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z
) {
	uint32 imageId = imageIdTrack | wooden_rc_get_track_colour();
	uint32 railsImageId = imageIdRails | wooden_rc_get_rails_colour();

	sub_98197C_rotated(direction, imageId, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset, bound_box_offset_x, bound_box_offset_y, bound_box_offset_z);
	sub_98199C_rotated(direction, railsImageId, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset, bound_box_offset_x, bound_box_offset_y, bound_box_offset_z);
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
	wooden_rc_track_paint(imageIds[direction][isChained], railsImageIds[direction][isChained], direction, 0, 2, 32, 25, 2, height, 0, 3, height);
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
		wooden_rc_track_paint(
			_wooden_rc_block_brakes_image_ids[direction][0],
			_wooden_rc_block_brakes_image_ids[direction][1], direction, 0, 2, 32, 27, 2, height, 0, 2, height);
	} else {
		wooden_rc_track_paint(
			stationImageIds[direction][0],
			stationImageIds[direction][1], direction, 0, 2, 32, 27, 2, height, 0, 2, height);
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
			{ SPR_WOODEN_RC_25_DEG_SW_NE, SPR_WOODEN_RC_25_DEG_RAILS_SW_NE, 0, 0 },
			{ SPR_WOODEN_RC_25_DEG_NW_SE, SPR_WOODEN_RC_25_DEG_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NW_SE },
			{ SPR_WOODEN_RC_25_DEG_NE_SW, SPR_WOODEN_RC_25_DEG_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NE_SW },
			{ SPR_WOODEN_RC_25_DEG_SE_NW, SPR_WOODEN_RC_25_DEG_RAILS_SE_NW, 0, 0 },
		},
		{
			{ SPR_WOODEN_RC_25_DEG_CHAIN_SW_NE, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SW_NE, 0, 0 },
			{ SPR_WOODEN_RC_25_DEG_CHAIN_NW_SE, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NW_SE },
			{ SPR_WOODEN_RC_25_DEG_CHAIN_NE_SW, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NE_SW },
			{ SPR_WOODEN_RC_25_DEG_CHAIN_SE_NW, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SE_NW, 0, 0 },
		}
	};

	uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
	wooden_rc_track_paint(imageIds[isChained][direction][0], imageIds[isChained][direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
	if (direction == 1 || direction == 2) {
		wooden_rc_track_paint(imageIds[isChained][direction][2], imageIds[isChained][direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
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

static void wooden_rc_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][2] = {
		{ SPR_WOODEN_RC_60_DEG_SW_NE, SPR_WOODEN_RC_60_DEG_RAILS_SW_NE },
		{ SPR_WOODEN_RC_60_DEG_NW_SE, SPR_WOODEN_RC_60_DEG_RAILS_NW_SE },
		{ SPR_WOODEN_RC_60_DEG_NE_SW, SPR_WOODEN_RC_60_DEG_RAILS_NE_SW },
		{ SPR_WOODEN_RC_60_DEG_SE_NW, SPR_WOODEN_RC_60_DEG_RAILS_SE_NW },
	};

	if (direction == 0 || direction == 3) {
		wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
	} else {
		wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 2, 24, 93, height, 28, 4, height - 16);
	}
	wooden_a_supports_paint_setup(direction & 1, 21 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 56, TUNNEL_8);
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 104, 0x20);
}

static void wooden_rc_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[2][4][4] = {
		{
			{ SPR_WOODEN_RC_FLAT_TO_25_DEG_SW_NE, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_SW_NE, 0, 0 },
			{ SPR_WOODEN_RC_FLAT_TO_25_DEG_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_FRONT_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_FRONT_NW_SE },
			{ SPR_WOODEN_RC_FLAT_TO_25_DEG_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_FRONT_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_FRONT_NE_SW },
			{ SPR_WOODEN_RC_FLAT_TO_25_DEG_SE_NW, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_SE_NW, 0, 0 },
		},
		{
			{ SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_SW_NE, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_SW_NE, 0, 0 },
			{ SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_FRONT_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_FRONT_NW_SE },
			{ SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_FRONT_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_FRONT_NE_SW },
			{ SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_SE_NW, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_SE_NW, 0, 0 },
		}
	};

	uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
	wooden_rc_track_paint(imageIds[isChained][direction][0], imageIds[isChained][direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
	if (direction == 1 || direction == 2) {
		wooden_rc_track_paint(imageIds[isChained][direction][2], imageIds[isChained][direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
	}
	wooden_a_supports_paint_setup(direction & 1, 1 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void wooden_rc_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][4] = {
		{ SPR_WOODEN_RC_25_DEG_TO_60_DEG_SW_NE, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_SW_NE, 0, 0 },
		{ SPR_WOODEN_RC_25_DEG_TO_60_DEG_NW_SE, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_TO_60_DEG_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_FRONT_NW_SE },
		{ SPR_WOODEN_RC_25_DEG_TO_60_DEG_NE_SW, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_TO_60_DEG_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_FRONT_NE_SW },
		{ SPR_WOODEN_RC_25_DEG_TO_60_DEG_SE_NW, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_SE_NW, 0, 0 },
	};

	if (direction == 0 || direction == 3) {
		wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
	} else {
		wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 2, 24, 43, height, 28, 4, height + 2);
		wooden_rc_track_paint(imageIds[direction][2], imageIds[direction][3], direction, 0, 0, 32, 2, 43, height, 0, 4, height);
	}
	wooden_a_supports_paint_setup(direction & 1, 13 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_8);
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 72, 0x20);
}

static void wooden_rc_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][4] = {
		{ SPR_WOODEN_RC_60_DEG_TO_25_DEG_SW_NE, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_SW_NE, 0, 0 },
		{ SPR_WOODEN_RC_60_DEG_TO_25_DEG_NW_SE, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_NW_SE, SPR_WOODEN_RC_60_DEG_TO_25_DEG_FRONT_NW_SE, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_FRONT_NW_SE },
		{ SPR_WOODEN_RC_60_DEG_TO_25_DEG_NE_SW, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_NE_SW, SPR_WOODEN_RC_60_DEG_TO_25_DEG_FRONT_NE_SW, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_FRONT_NE_SW },
		{ SPR_WOODEN_RC_60_DEG_TO_25_DEG_SE_NW, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_SE_NW, 0, 0 },
	};

	if (direction == 0 || direction == 3) {
		wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
	} else {
		wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 24, 1, 61, height, 4, 28, height - 16);
		wooden_rc_track_paint(imageIds[direction][2], imageIds[direction][3], direction, 0, 0, 32, 2, 43, height, 0, 4, height);
	}
	wooden_a_supports_paint_setup(direction & 1, 17 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_8);
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 72, 0x20);
}

static void wooden_rc_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[2][4][4] = {
		{
			{ SPR_WOODEN_RC_25_DEG_TO_FLAT_SW_NE, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_SW_NE, 0, 0 },
			{ SPR_WOODEN_RC_25_DEG_TO_FLAT_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_FRONT_NW_SE },
			{ SPR_WOODEN_RC_25_DEG_TO_FLAT_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_FRONT_NE_SW },
			{ SPR_WOODEN_RC_25_DEG_TO_FLAT_SE_NW, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_SE_NW, 0, 0 },
		},
		{
			{ SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_SW_NE, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_SW_NE, 0, 0 },
			{ SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_FRONT_NW_SE },
			{ SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_FRONT_NE_SW },
			{ SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_SE_NW, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_SE_NW, 0, 0 },
		}
	};

	uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
	wooden_rc_track_paint(imageIds[isChained][direction][0], imageIds[isChained][direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
	if (direction == 1 || direction == 2) {
		wooden_rc_track_paint(imageIds[isChained][direction][2], imageIds[isChained][direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
	}
	wooden_a_supports_paint_setup(direction & 1, 5 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void wooden_rc_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	wooden_rc_track_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void wooden_rc_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	wooden_rc_track_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void wooden_rc_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	wooden_rc_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void wooden_rc_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	wooden_rc_track_60_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void wooden_rc_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	wooden_rc_track_25_deg_up_to_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void wooden_rc_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	wooden_rc_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void wooden_rc_track_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][2] = {
		{ SPR_WOODEN_RC_BRAKES_SW_NE, SPR_WOODEN_RC_BRAKES_RAILS_SW_NE },
		{ SPR_WOODEN_RC_BRAKES_NW_SE, SPR_WOODEN_RC_BRAKES_RAILS_NW_SE },
		{ SPR_WOODEN_RC_BRAKES_SW_NE, SPR_WOODEN_RC_BRAKES_RAILS_SW_NE },
		{ SPR_WOODEN_RC_BRAKES_NW_SE, SPR_WOODEN_RC_BRAKES_RAILS_NW_SE },
	};

	wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 2, 32, 25, 2, height, 0, 3, height);
	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void wooden_rc_track_block_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	wooden_rc_track_paint(
		_wooden_rc_block_brakes_image_ids[direction][0],
		_wooden_rc_block_brakes_image_ids[direction][1], direction, 0, 2, 32, 25, 2, height, 0, 3, height);
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
	case TRACK_ELEM_60_DEG_UP:
		return wooden_rc_track_60_deg_up;
	case TRACK_ELEM_FLAT_TO_25_DEG_UP:
		return wooden_rc_track_flat_to_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
		return wooden_rc_track_25_deg_up_to_60_deg_up;
	case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
		return wooden_rc_track_60_deg_up_to_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_FLAT:
		return wooden_rc_track_25_deg_up_to_flat;
	case TRACK_ELEM_25_DEG_DOWN:
		return wooden_rc_track_25_deg_down;
	case TRACK_ELEM_60_DEG_DOWN:
		return wooden_rc_track_60_deg_down;
	case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
		return wooden_rc_track_flat_to_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
		return wooden_rc_track_25_deg_down_to_60_deg_down;
	case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
		return wooden_rc_track_60_deg_down_to_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
		return wooden_rc_track_25_deg_down_to_flat;

	case TRACK_ELEM_BRAKES:
		return wooden_rc_track_brakes;
	case TRACK_ELEM_BLOCK_BRAKES:
		return wooden_rc_track_block_brakes;
	}
	return NULL;
}
