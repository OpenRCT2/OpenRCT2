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

/** rct2: 0x008B08D0 */
static void mine_ride_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19338, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19339, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void mine_ride_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][3] = {
		{ 19338, SPR_STATION_BASE_A_SW_NE },
		{ 19339, SPR_STATION_BASE_A_NW_SE },
		{ 19338, SPR_STATION_BASE_A_SW_NE },
		{ 19339, SPR_STATION_BASE_A_NW_SE },
	};

	sub_98197C_rotated(direction, imageIds[direction][0] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 6, height + 3);
	sub_98196C_rotated(direction, imageIds[direction][1] | gTrackColours[SCHEME_MISC], 0, 0, 32, 32, 1, height);
	track_paint_util_draw_station_metal_supports_2(direction, height, gTrackColours[SCHEME_SUPPORTS], 0);
	track_paint_util_draw_station_2(rideIndex, trackSequence, direction, height, mapElement, 9, 11);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008B08E0 */
static void mine_ride_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19388, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19389, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19390, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19391, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008B08F0 */
static void mine_ride_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19380, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19381, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19382, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19383, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008B0900 */
static void mine_ride_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19384, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19385, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19386, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19387, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008B0910 */
static void mine_ride_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	mine_ride_track_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0920 */
static void mine_ride_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	mine_ride_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0930 */
static void mine_ride_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	mine_ride_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0940 */
static void mine_ride_track_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19427, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19432, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19437, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19422, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19426, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19431, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19436, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19421, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19425, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19430, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19435, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19420, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 5:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19424, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19429, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19434, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19419, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19423, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19428, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19433, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19418, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0950 */
static void mine_ride_track_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	mine_ride_track_left_quarter_turn_5(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008B0960 */
static void mine_ride_track_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19340, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19348, 0, 0, 32, 1, 26, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19341, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19349, 0, 0, 32, 1, 26, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19342, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19343, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008B0970 */
static void mine_ride_track_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19344, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19345, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19346, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19350, 0, 0, 32, 1, 26, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19347, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19351, 0, 0, 32, 1, 26, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008B0980 */
static void mine_ride_track_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19346, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19350, 0, 0, 32, 1, 26, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19347, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19351, 0, 0, 32, 1, 26, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19344, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19345, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008B0990 */
static void mine_ride_track_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19342, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19343, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19340, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19348, 0, 0, 32, 1, 26, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19341, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19349, 0, 0, 32, 1, 26, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008B09A0 */
static void mine_ride_track_banked_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19447, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19458, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19452, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19457, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19442, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19446, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19451, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19456, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19441, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19445, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19450, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19455, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19440, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 5:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19444, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19449, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19454, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19439, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19443, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19448, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19453, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19459, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19438, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B09B0 */
static void mine_ride_track_banked_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	mine_ride_track_banked_left_quarter_turn_5(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008B09C0 */
static void mine_ride_track_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19352, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19356, 0, 0, 32, 1, 34, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19353, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19357, 0, 0, 32, 1, 34, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19354, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19355, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008B09D0 */
static void mine_ride_track_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19358, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19359, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19360, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19362, 0, 0, 32, 1, 34, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19361, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19363, 0, 0, 32, 1, 34, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008B09E0 */
static void mine_ride_track_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19364, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19368, 0, 0, 32, 1, 34, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19365, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19369, 0, 0, 32, 1, 34, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19366, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19367, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008B09F0 */
static void mine_ride_track_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19370, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19371, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19372, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19374, 0, 0, 32, 1, 34, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19373, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19375, 0, 0, 32, 1, 34, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008B0A00 */
static void mine_ride_track_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	mine_ride_track_25_deg_up_to_right_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0A10 */
static void mine_ride_track_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	mine_ride_track_25_deg_up_to_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0A20 */
static void mine_ride_track_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	mine_ride_track_right_bank_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0A30 */
static void mine_ride_track_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	mine_ride_track_left_bank_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0A40 */
static void mine_ride_track_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19376, 0, 0, 32, 1, 26, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19377, 0, 0, 32, 1, 26, height, 0, 27, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19378, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19379, 0, 0, 32, 20, 3, height, 0, 6, height);
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
		break;
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008B0A50 */
static void mine_ride_track_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	mine_ride_track_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0A60 */
static void mine_ride_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19460, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19464, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19463, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19467, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19461, 0, 0, 32, 26, 3, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 5, 3, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19465, 0, 0, 32, 26, 3, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 6, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19462, 0, 0, 32, 26, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19466, 0, 0, 32, 26, 3, height, 0, 6, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19462, 0, 0, 32, 26, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19466, 0, 0, 32, 26, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19461, 0, 0, 32, 26, 3, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 5, 3, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19465, 0, 0, 32, 26, 3, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 6, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19463, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19467, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19460, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19464, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 1:
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
		case 2:
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0A70 */
static void mine_ride_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19468, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19472, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19471, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19475, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19469, 0, 0, 32, 26, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 8, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19473, 0, 0, 32, 26, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 7, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19470, 0, 0, 32, 26, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19474, 0, 0, 32, 26, 3, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19470, 0, 0, 32, 26, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19474, 0, 0, 32, 26, 3, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19469, 0, 0, 32, 26, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 8, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19473, 0, 0, 32, 26, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 7, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19471, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19475, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19468, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19472, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 1:
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
		case 2:
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0AB0 */
static void mine_ride_track_left_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19397, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19400, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19403, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19394, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19396, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19399, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19402, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19393, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19395, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19398, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19401, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19392, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0AC0 */
static void mine_ride_track_right_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	mine_ride_track_left_quarter_turn_3(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008B0AD0 */
static void mine_ride_track_left_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19409, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19416, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19412, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19415, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19406, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19408, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19411, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19414, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19405, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19407, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19410, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19413, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19417, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19404, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0AE0 */
static void mine_ride_track_right_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	mine_ride_track_left_quarter_turn_3_bank(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008B0AF0 */
static void mine_ride_track_left_half_banked_helix_up_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19495, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19502, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19498, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19501, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19492, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19494, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19497, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19500, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19491, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19493, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19496, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19499, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19503, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 15, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19490, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height + 8, TUNNEL_6);
			break;
		case 3:
			paint_util_push_tunnel_left(height + 8, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19492, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19495, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19502, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19498, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19501, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
		case 1:
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 5:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19491, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19494, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19497, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19500, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 7:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19490, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19493, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19496, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19499, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19503, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 15, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0B00 */
static void mine_ride_track_right_half_banked_helix_up_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19476, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19479, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19482, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19485, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19489, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 11, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19477, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19480, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19483, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19486, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19478, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19481, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19488, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 15, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19484, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19487, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 8, TUNNEL_6);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 8, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19479, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19482, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19485, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19489, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 11, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19476, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 5:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19480, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19483, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19486, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19477, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 7:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19481, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19488, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 15, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19484, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19487, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19478, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0B10 */
static void mine_ride_track_left_half_banked_helix_down_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	if (trackSequence >= 4) {
		trackSequence -= 4;
		direction = (direction - 1) & 3;
	}
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	mine_ride_track_right_half_banked_helix_up_small(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008B0B20 */
static void mine_ride_track_right_half_banked_helix_down_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	if (trackSequence >= 4) {
		trackSequence -= 4;
		direction = (direction + 1) & 3;
	}
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	mine_ride_track_left_half_banked_helix_up_small(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008B0B30 */
static void mine_ride_track_left_half_banked_helix_up_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19535, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19546, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19540, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19545, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19530, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19534, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19539, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19544, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19529, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19533, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19538, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19543, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19528, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 5:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19532, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19537, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19542, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19527, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19531, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19536, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19541, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19547, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19526, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height + 8, TUNNEL_6);
			break;
		case 3:
			paint_util_push_tunnel_left(height + 8, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 7:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19530, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19535, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19546, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19540, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19545, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
		case 1:
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 8:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 9:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19529, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19534, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19539, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19544, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 10:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19528, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19533, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19538, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19543, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 11:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 12:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19527, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19532, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19537, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19542, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 13:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19526, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19531, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19536, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19541, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19547, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0B40 */
static void mine_ride_track_right_half_banked_helix_up_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19504, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19509, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19514, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19519, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19525, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19505, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19510, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19515, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19520, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19506, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19511, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19516, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19521, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 5:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19507, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19512, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19517, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19522, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19508, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19513, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19524, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19518, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19523, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 8, TUNNEL_6);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 8, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 7:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19509, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19514, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19519, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19525, 0, 0, 1, 32, 26, height, 27, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19504, 0, 0, 20, 32, 3, height, 6, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_6);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_6);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 8:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 9:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19510, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19515, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19520, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19505, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 10:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19511, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19516, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19521, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19506, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 11:
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 12:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19512, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19517, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19522, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19507, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 13:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19513, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19524, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19518, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19523, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19508, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0B50 */
static void mine_ride_track_left_half_banked_helix_down_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	if (trackSequence >= 7) {
		trackSequence -= 7;
		direction = (direction - 1) & 3;
	}
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	mine_ride_track_right_half_banked_helix_up_large(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008B0B60 */
static void mine_ride_track_right_half_banked_helix_down_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	if (trackSequence >= 7) {
		trackSequence -= 7;
		direction = (direction + 1) & 3;
	}
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	mine_ride_track_left_half_banked_helix_up_large(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008B0B70 */
static void mine_ride_track_on_ride_photo(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98196C_rotated(direction, 0x20000000 | 22432, 0, 0, 32, 32, 1, height);
		metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 5, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 8, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19338, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
		break;
	case 1:
		sub_98196C_rotated(direction, 0x20000000 | 22432, 0, 0, 32, 32, 1, height);
		metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 6, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 7, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19339, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
		break;
	case 2:
		sub_98196C_rotated(direction, 0x20000000 | 22432, 0, 0, 32, 32, 1, height);
		metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 5, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 8, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19338, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
		break;
	case 3:
		sub_98196C_rotated(direction, 0x20000000 | 22432, 0, 0, 32, 32, 1, height);
		metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 6, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 7, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19339, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
		break;
	}
	track_paint_util_onride_photo_paint(direction, height + 3, mapElement);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008B0B90 */
static void mine_ride_track_left_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19564, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19568, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19572, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19576, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19565, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19569, 0, 0, 34, 16, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19573, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19577, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19566, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19570, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19574, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19578, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19567, 0, 0, 16, 16, 3, height, 16, 16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19571, 0, 0, 16, 18, 3, height, 0, 16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19575, 0, 0, 16, 16, 3, height, 0, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19579, 0, 0, 16, 16, 3, height, 16, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0BA0 */
static void mine_ride_track_right_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19548, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19552, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19556, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19560, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19549, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19553, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19557, 0, 0, 34, 16, 3, height, 0, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19561, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19550, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19554, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19558, 0, 0, 28, 28, 3, height, 4, 4, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19562, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19551, 0, 0, 16, 16, 3, height, 16, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19555, 0, 0, 16, 16, 3, height, 0, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19559, 0, 0, 16, 18, 3, height, 0, 16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19563, 0, 0, 16, 16, 3, height, 16, 16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0BB0 */
static void mine_ride_track_left_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
	mine_ride_track_right_eighth_to_diag(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0BC0 */
static void mine_ride_track_right_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
	mine_ride_track_left_eighth_to_diag(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008B0BD0 */
static void mine_ride_track_left_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19596, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19600, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19604, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19608, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19597, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19601, 0, 0, 34, 16, 0, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19605, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19609, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19598, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19602, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19606, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19610, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19599, 0, 0, 16, 16, 3, height, 16, 16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19603, 0, 0, 16, 18, 0, height, 0, 16, height + 27);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19607, 0, 0, 16, 16, 3, height, 0, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19611, 0, 0, 16, 16, 3, height, 16, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0BE0 */
static void mine_ride_track_right_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19580, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19584, 0, 0, 32, 20, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19588, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19592, 0, 0, 32, 1, 26, height, 0, 27, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19581, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19585, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19589, 0, 0, 34, 16, 0, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19593, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19582, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19586, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19590, 0, 0, 28, 28, 0, height, 4, 4, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19594, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19583, 0, 0, 16, 16, 3, height, 16, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19587, 0, 0, 16, 16, 3, height, 0, 0, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19591, 0, 0, 16, 18, 0, height, 0, 16, height + 27);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19595, 0, 0, 16, 16, 3, height, 16, 16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0BF0 */
static void mine_ride_track_left_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
	mine_ride_track_right_eighth_bank_to_diag(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0C00 */
static void mine_ride_track_right_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
	mine_ride_track_left_eighth_bank_to_diag(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008B0B80 */
static void mine_ride_track_diag_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19615, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19612, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19614, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19613, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0C30 */
static void mine_ride_track_diag_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19627, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19624, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19626, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19625, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

/** rct2: 0x008B0C10 */
static void mine_ride_track_diag_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19619, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19616, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19618, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19617, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

/** rct2: 0x008B0C20 */
static void mine_ride_track_diag_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19623, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19620, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19622, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19621, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

/** rct2: 0x008B0C60 */
static void mine_ride_track_diag_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19625, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19626, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19624, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19627, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

/** rct2: 0x008B0C40 */
static void mine_ride_track_diag_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19621, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19622, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19620, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19623, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		break;
	}

	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008B0C50 */
static void mine_ride_track_diag_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19617, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19618, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19616, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19619, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

/** rct2: 0x008B0C90 */
static void mine_ride_track_diag_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19635, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19632, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19636, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19634, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19633, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0CA0 */
static void mine_ride_track_diag_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19640, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19637, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19639, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19641, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19638, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0CB0 */
static void mine_ride_track_diag_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19638, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19639, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19641, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19637, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19640, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0CC0 */
static void mine_ride_track_diag_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19633, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19634, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19632, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19636, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19635, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0CF0 */
static void mine_ride_track_diag_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19655, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19652, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19656, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19654, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19653, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

/** rct2: 0x008B0D00 */
static void mine_ride_track_diag_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19660, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19657, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19659, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19661, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19658, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

/** rct2: 0x008B0CD0 */
static void mine_ride_track_diag_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19645, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19642, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19646, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19644, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19643, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

/** rct2: 0x008B0CE0 */
static void mine_ride_track_diag_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19650, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19647, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19649, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19651, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19648, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

/** rct2: 0x008B0D10 */
static void mine_ride_track_diag_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19648, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19649, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19651, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19647, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19650, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		break;
	}

	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008B0D20 */
static void mine_ride_track_diag_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19643, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19644, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19642, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19646, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19645, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		break;
	}

	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008B0D30 */
static void mine_ride_track_diag_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19658, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19659, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19661, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19657, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19660, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

/** rct2: 0x008B0D40 */
static void mine_ride_track_diag_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19653, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19654, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19652, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19656, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19655, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

/** rct2: 0x008B0C70 */
static void mine_ride_track_diag_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19631, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19628, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19630, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19629, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

/** rct2: 0x008B0C80 */
static void mine_ride_track_diag_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19629, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19630, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19628, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19631, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

TRACK_PAINT_FUNCTION get_track_paint_function_mine_ride(sint32 trackType, sint32 direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT:
		return mine_ride_track_flat;
	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
		return mine_ride_track_station;
	case TRACK_ELEM_25_DEG_UP:
		return mine_ride_track_25_deg_up;
	case TRACK_ELEM_FLAT_TO_25_DEG_UP:
		return mine_ride_track_flat_to_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_FLAT:
		return mine_ride_track_25_deg_up_to_flat;
	case TRACK_ELEM_25_DEG_DOWN:
		return mine_ride_track_25_deg_down;
	case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
		return mine_ride_track_flat_to_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
		return mine_ride_track_25_deg_down_to_flat;
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
		return mine_ride_track_left_quarter_turn_5;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
		return mine_ride_track_right_quarter_turn_5;
	case TRACK_ELEM_FLAT_TO_LEFT_BANK:
		return mine_ride_track_flat_to_left_bank;
	case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
		return mine_ride_track_flat_to_right_bank;
	case TRACK_ELEM_LEFT_BANK_TO_FLAT:
		return mine_ride_track_left_bank_to_flat;
	case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
		return mine_ride_track_right_bank_to_flat;
	case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
		return mine_ride_track_banked_left_quarter_turn_5;
	case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
		return mine_ride_track_banked_right_quarter_turn_5;
	case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
		return mine_ride_track_left_bank_to_25_deg_up;
	case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
		return mine_ride_track_right_bank_to_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
		return mine_ride_track_25_deg_up_to_left_bank;
	case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
		return mine_ride_track_25_deg_up_to_right_bank;
	case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
		return mine_ride_track_left_bank_to_25_deg_down;
	case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
		return mine_ride_track_right_bank_to_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
		return mine_ride_track_25_deg_down_to_left_bank;
	case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
		return mine_ride_track_25_deg_down_to_right_bank;
	case TRACK_ELEM_LEFT_BANK:
		return mine_ride_track_left_bank;
	case TRACK_ELEM_RIGHT_BANK:
		return mine_ride_track_right_bank;
	case TRACK_ELEM_S_BEND_LEFT:
		return mine_ride_track_s_bend_left;
	case TRACK_ELEM_S_BEND_RIGHT:
		return mine_ride_track_s_bend_right;
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
		return mine_ride_track_left_quarter_turn_3;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
		return mine_ride_track_right_quarter_turn_3;
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
		return mine_ride_track_left_quarter_turn_3_bank;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
		return mine_ride_track_right_quarter_turn_3_bank;
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
		return mine_ride_track_left_half_banked_helix_up_small;
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
		return mine_ride_track_right_half_banked_helix_up_small;
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
		return mine_ride_track_left_half_banked_helix_down_small;
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
		return mine_ride_track_right_half_banked_helix_down_small;
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
		return mine_ride_track_left_half_banked_helix_up_large;
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
		return mine_ride_track_right_half_banked_helix_up_large;
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
		return mine_ride_track_left_half_banked_helix_down_large;
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
		return mine_ride_track_right_half_banked_helix_down_large;
	case TRACK_ELEM_ON_RIDE_PHOTO:
		return mine_ride_track_on_ride_photo;
	case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
		return mine_ride_track_left_eighth_to_diag;
	case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
		return mine_ride_track_right_eighth_to_diag;
	case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
		return mine_ride_track_left_eighth_to_orthogonal;
	case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
		return mine_ride_track_right_eighth_to_orthogonal;
	case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
		return mine_ride_track_left_eighth_bank_to_diag;
	case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
		return mine_ride_track_right_eighth_bank_to_diag;
	case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
		return mine_ride_track_left_eighth_bank_to_orthogonal;
	case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
		return mine_ride_track_right_eighth_bank_to_orthogonal;
	case TRACK_ELEM_DIAG_FLAT:
		return mine_ride_track_diag_flat;
	case TRACK_ELEM_DIAG_25_DEG_UP:
		return mine_ride_track_diag_25_deg_up;
	case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
		return mine_ride_track_diag_flat_to_25_deg_up;
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
		return mine_ride_track_diag_25_deg_up_to_flat;
	case TRACK_ELEM_DIAG_25_DEG_DOWN:
		return mine_ride_track_diag_25_deg_down;
	case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
		return mine_ride_track_diag_flat_to_25_deg_down;
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
		return mine_ride_track_diag_25_deg_down_to_flat;
	case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
		return mine_ride_track_diag_flat_to_left_bank;
	case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
		return mine_ride_track_diag_flat_to_right_bank;
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
		return mine_ride_track_diag_left_bank_to_flat;
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
		return mine_ride_track_diag_right_bank_to_flat;
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
		return mine_ride_track_diag_left_bank_to_25_deg_up;
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
		return mine_ride_track_diag_right_bank_to_25_deg_up;
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
		return mine_ride_track_diag_25_deg_up_to_left_bank;
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
		return mine_ride_track_diag_25_deg_up_to_right_bank;
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
		return mine_ride_track_diag_left_bank_to_25_deg_down;
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
		return mine_ride_track_diag_right_bank_to_25_deg_down;
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
		return mine_ride_track_diag_25_deg_down_to_left_bank;
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
		return mine_ride_track_diag_25_deg_down_to_right_bank;
	case TRACK_ELEM_DIAG_LEFT_BANK:
		return mine_ride_track_diag_left_bank;
	case TRACK_ELEM_DIAG_RIGHT_BANK:
		return mine_ride_track_diag_right_bank;
	}
	return NULL;
}
