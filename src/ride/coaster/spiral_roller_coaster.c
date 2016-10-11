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

static void spiral_rc_track_left_curved_lift_hill(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19333, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(1, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19335, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(2, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19337, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(1, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19331, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(2, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19332, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(2, 4, 17, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19334, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(1, 4, 17, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19336, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(2, 4, 17, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19330, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(1, 4, 17, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height + 16, TUNNEL_8);
			break;
		case 3:
			paint_util_push_tunnel_left(height + 16, TUNNEL_8);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void spiral_rc_track_right_curved_lift_hill(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19322, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(1, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19324, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(2, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19326, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(1, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19328, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(2, 4, 14, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19323, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(2, 4, 17, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19325, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(1, 4, 17, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19327, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(2, 4, 17, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 19329, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(1, 4, 17, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 16, TUNNEL_8);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 16, TUNNEL_8);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

TRACK_PAINT_FUNCTION get_track_paint_function_spiral_rc(int trackType, int direction)
{
	switch (trackType) {

	// Use Mini Roller Coaster
	case TRACK_ELEM_FLAT:
	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
	case TRACK_ELEM_25_DEG_UP:
	case TRACK_ELEM_60_DEG_UP:
	case TRACK_ELEM_FLAT_TO_25_DEG_UP:
	case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
	case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
	case TRACK_ELEM_25_DEG_UP_TO_FLAT:
	case TRACK_ELEM_25_DEG_DOWN:
	case TRACK_ELEM_60_DEG_DOWN:
	case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
	case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
	case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
	case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
	case TRACK_ELEM_FLAT_TO_LEFT_BANK:
	case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
	case TRACK_ELEM_LEFT_BANK_TO_FLAT:
	case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
	case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
	case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
	case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
	case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
	case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
	case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
	case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
	case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
	case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
	case TRACK_ELEM_LEFT_BANK:
	case TRACK_ELEM_RIGHT_BANK:
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
	case TRACK_ELEM_S_BEND_LEFT:
	case TRACK_ELEM_S_BEND_RIGHT:
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
	case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP:
	case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
	case TRACK_ELEM_BRAKES:
	case TRACK_ELEM_25_DEG_UP_LEFT_BANKED:
	case TRACK_ELEM_25_DEG_UP_RIGHT_BANKED:
	case TRACK_ELEM_ON_RIDE_PHOTO:
	case TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED:
	case TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED:
	case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
	case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
	case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
	case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
	case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
	case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
	case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
	case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
	case TRACK_ELEM_DIAG_FLAT:
	case TRACK_ELEM_DIAG_25_DEG_UP:
	case TRACK_ELEM_DIAG_60_DEG_UP:
	case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
	case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
	case TRACK_ELEM_DIAG_25_DEG_DOWN:
	case TRACK_ELEM_DIAG_60_DEG_DOWN:
	case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
	case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
	case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
	case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
	case TRACK_ELEM_DIAG_LEFT_BANK:
	case TRACK_ELEM_DIAG_RIGHT_BANK:
	case TRACK_ELEM_BLOCK_BRAKES:
	case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
	case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
	case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
	case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
	case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
	case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
	case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP:
	case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP:
	case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP:
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP:
	case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN:
	case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN:
	case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
	case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP:
	case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
	case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT:
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT:
	case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
	case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
	case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT:
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT:
	case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP:
	case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
	case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT:
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT:
	case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
	case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
	case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT:
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT:
		return get_track_paint_function_mini_rc(trackType, direction);

	case TRACK_ELEM_LEFT_CURVED_LIFT_HILL:
		return spiral_rc_track_left_curved_lift_hill;
	case TRACK_ELEM_RIGHT_CURVED_LIFT_HILL:
		return spiral_rc_track_right_curved_lift_hill;
	}
	return NULL;
}
