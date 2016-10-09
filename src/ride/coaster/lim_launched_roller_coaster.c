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

static void lim_launched_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (track_element_is_lift_hill(mapElement)) {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15006, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15007, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15008, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15009, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	} else {
		switch (direction) {
		case 0:
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15004, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15005, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void lim_launched_rc_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (track_element_is_lift_hill(mapElement)) {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15060, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15061, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15062, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15063, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	} else {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15032, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15033, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15034, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15035, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void lim_launched_rc_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (track_element_is_lift_hill(mapElement)) {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15076, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15077, 0, 0, 32, 1, 98, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15078, 0, 0, 32, 1, 98, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15079, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 32, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	} else {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15048, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15049, 0, 0, 32, 1, 98, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15050, 0, 0, 32, 1, 98, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15051, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 32, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 56, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 104, 0x20);
}

static void lim_launched_rc_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (track_element_is_lift_hill(mapElement)) {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15052, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15053, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15054, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15055, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	} else {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15024, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15025, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15026, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15027, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void lim_launched_rc_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (track_element_is_lift_hill(mapElement)) {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15064, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15065, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15068, 0, 0, 32, 1, 66, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15066, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15069, 0, 0, 32, 1, 66, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15067, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	} else {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15036, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15037, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15040, 0, 0, 32, 1, 66, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15038, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15041, 0, 0, 32, 1, 66, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15039, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 72, 0x20);
}

static void lim_launched_rc_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (track_element_is_lift_hill(mapElement)) {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15070, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15071, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15074, 0, 0, 32, 1, 66, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15072, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15075, 0, 0, 32, 1, 66, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15073, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	} else {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15042, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15043, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15046, 0, 0, 32, 1, 66, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15044, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15047, 0, 0, 32, 1, 66, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15045, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 72, 0x20);
}

static void lim_launched_rc_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (track_element_is_lift_hill(mapElement)) {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15056, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15057, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15058, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15059, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	} else {
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15028, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15029, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15030, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15031, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
			metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		}
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void lim_launched_rc_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_60_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_25_deg_up_to_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15183, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15188, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15193, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15178, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15182, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15187, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15192, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15177, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15181, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15186, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15191, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15176, 0, 0, 16, 16, 3, height, 0, 0, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15180, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15185, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15190, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15175, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15179, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15184, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15189, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15174, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	lim_launched_rc_track_left_quarter_turn_5(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15080, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15092, 0, 0, 32, 1, 26, height, 0, 27, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15081, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15093, 0, 0, 32, 1, 26, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15082, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15083, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void lim_launched_rc_track_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15084, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15085, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15086, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15094, 0, 0, 32, 1, 26, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15087, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15095, 0, 0, 32, 1, 26, height, 0, 27, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void lim_launched_rc_track_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15086, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15094, 0, 0, 32, 1, 26, height, 0, 27, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15087, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15095, 0, 0, 32, 1, 26, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15084, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15085, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void lim_launched_rc_track_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15082, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15083, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15080, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15092, 0, 0, 32, 1, 26, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15081, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15093, 0, 0, 32, 1, 26, height, 0, 27, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void lim_launched_rc_track_banked_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15203, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15214, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15208, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15213, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15198, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15202, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15207, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15212, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15197, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15201, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15206, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15211, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15196, 0, 0, 16, 16, 3, height, 0, 0, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15200, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15205, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15210, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15195, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15199, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15204, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15209, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15215, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15194, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_banked_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	lim_launched_rc_track_banked_left_quarter_turn_5(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15096, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15112, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15097, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15113, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15098, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15099, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void lim_launched_rc_track_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15100, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15101, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15102, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15114, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15103, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15115, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void lim_launched_rc_track_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15104, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15116, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15105, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15117, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15106, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15107, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void lim_launched_rc_track_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15108, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15109, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15110, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15118, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15111, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15119, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void lim_launched_rc_track_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_25_deg_up_to_right_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_25_deg_up_to_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_right_bank_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_left_bank_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15088, 0, 0, 32, 1, 26, height, 0, 27, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15089, 0, 0, 32, 1, 26, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15090, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15091, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void lim_launched_rc_track_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15296, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15301, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15306, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15311, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15297, 0, 0, 32, 16, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15302, 0, 0, 32, 16, 3, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15307, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15312, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15298, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15303, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15308, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15313, 0, 0, 16, 16, 3, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 64, 0x20);
		break;
	case 4:
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 5:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15299, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15304, 0, 0, 16, 32, 3, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15309, 0, 0, 16, 32, 3, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15314, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15300, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15305, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15310, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15315, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height + 8, TUNNEL_2);
			break;
		case 3:
			paint_util_push_tunnel_left(height + 8, TUNNEL_2);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15276, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15281, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15286, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15291, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15277, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15282, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15287, 0, 0, 32, 16, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15292, 0, 0, 32, 16, 3, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15278, 0, 0, 16, 16, 3, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15283, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15288, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15293, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 64, 0x20);
		break;
	case 4:
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 5:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15279, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15284, 0, 0, 16, 32, 3, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15289, 0, 0, 16, 32, 3, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15294, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15280, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15285, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15290, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15295, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 8, TUNNEL_2);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 8, TUNNEL_2);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	lim_launched_rc_track_right_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	lim_launched_rc_track_left_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15260, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15264, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15263, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15267, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15261, 0, 0, 32, 26, 3, height);
			metal_a_supports_paint_setup(0, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15265, 0, 0, 32, 26, 3, height);
			metal_a_supports_paint_setup(0, 6, 1, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15262, 0, 0, 32, 26, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15266, 0, 0, 32, 26, 3, height, 0, 6, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15262, 0, 0, 32, 26, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15266, 0, 0, 32, 26, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15261, 0, 0, 32, 26, 3, height);
			metal_a_supports_paint_setup(0, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15265, 0, 0, 32, 26, 3, height);
			metal_a_supports_paint_setup(0, 6, 1, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15263, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15267, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15260, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15264, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 1:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 2:
			paint_util_push_tunnel_left(height, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15268, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15272, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15271, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15275, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15269, 0, 0, 32, 26, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(0, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15273, 0, 0, 32, 26, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(0, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15270, 0, 0, 32, 26, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15274, 0, 0, 32, 26, 3, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15270, 0, 0, 32, 26, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15274, 0, 0, 32, 26, 3, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15269, 0, 0, 32, 26, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(0, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15273, 0, 0, 32, 26, 3, height, 0, 6, height);
			metal_a_supports_paint_setup(0, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15271, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15275, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15268, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15272, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 1:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 2:
			paint_util_push_tunnel_left(height, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_vertical_loop(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15388, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15396, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15395, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15403, 0, 6, 32, 20, 7, height);
			metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15389, 0, 0, 32, 26, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15397, 0, 14, 32, 2, 63, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15394, 0, 6, 32, 26, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15402, 0, 6, 32, 26, 3, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15390, 16, 0, 3, 16, 119, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15398, 12, 0, 3, 16, 119, height, 12, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15393, 10, 16, 4, 16, 119, height, 10, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15401, 16, 16, 2, 16, 119, height, 16, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 168, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15391, 0, 0, 32, 16, 3, height + 32);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15399, 0, 0, 32, 16, 3, height + 32);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15392, 0, 16, 32, 16, 3, height + 32);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15400, 0, 16, 32, 16, 3, height + 32);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 4:
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 5:
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15392, 0, 16, 32, 16, 3, height + 32);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15400, 0, 16, 32, 16, 3, height + 32);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15391, 0, 0, 32, 16, 3, height + 32);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15399, 0, 0, 32, 16, 3, height + 32);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 7:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15393, 10, 16, 4, 16, 119, height, 10, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15401, 16, 16, 2, 16, 119, height, 16, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15390, 16, 0, 3, 16, 119, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15398, 12, 0, 3, 16, 119, height, 12, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 168, 0x20);
		break;
	case 8:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15394, 0, 6, 32, 26, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15402, 0, 6, 32, 26, 3, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15389, 0, 0, 32, 26, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15397, 0, 14, 32, 2, 63, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 9:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15395, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15403, 0, 6, 32, 20, 7, height);
			metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15388, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15396, 0, 6, 32, 20, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 1:
			paint_util_push_tunnel_right(height - 8, TUNNEL_1);
			break;
		case 2:
			paint_util_push_tunnel_left(height - 8, TUNNEL_1);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_vertical_loop(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15419, 0, 6, 32, 20, 7, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15411, 0, 6, 32, 20, 3, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15412, 0, 6, 32, 20, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15404, 0, 6, 32, 20, 3, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15418, 0, 6, 32, 26, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15410, 0, 6, 32, 26, 3, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15413, 0, 14, 32, 2, 63, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15405, 0, 0, 32, 26, 3, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15417, 16, 16, 2, 16, 119, height, 16, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15409, 10, 16, 4, 16, 119, height, 10, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15414, 12, 0, 3, 16, 119, height, 12, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15406, 16, 0, 2, 16, 119, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 168, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15416, 0, 16, 32, 16, 3, height + 32);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15408, 0, 16, 32, 16, 3, height + 32);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15415, 0, 0, 32, 16, 3, height + 32);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15407, 0, 0, 32, 16, 3, height + 32);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 4:
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 5:
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15415, 0, 0, 32, 16, 3, height + 32);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15407, 0, 0, 32, 16, 3, height + 32);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15416, 0, 16, 32, 16, 3, height + 32);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15408, 0, 16, 32, 16, 3, height + 32);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 7:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15414, 12, 0, 3, 16, 119, height, 12, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15406, 16, 0, 2, 16, 119, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15417, 16, 16, 2, 16, 119, height, 16, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15409, 10, 16, 4, 16, 119, height, 10, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 168, 0x20);
		break;
	case 8:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15413, 0, 14, 32, 2, 63, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15405, 0, 0, 32, 26, 3, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15418, 0, 6, 32, 26, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15410, 0, 6, 32, 26, 3, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 9:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15412, 0, 6, 32, 20, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15404, 0, 6, 32, 20, 3, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15419, 0, 6, 32, 20, 7, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15411, 0, 6, 32, 20, 3, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 1:
			paint_util_push_tunnel_right(height - 8, TUNNEL_1);
			break;
		case 2:
			paint_util_push_tunnel_left(height - 8, TUNNEL_1);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15125, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15128, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15131, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15122, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15124, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15127, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15130, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15121, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15123, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15126, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15129, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15120, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	lim_launched_rc_track_left_quarter_turn_3(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15137, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15144, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15140, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15143, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15134, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15136, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15139, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15142, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15133, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15135, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15138, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15141, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15145, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15132, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	lim_launched_rc_track_left_quarter_turn_3_bank(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15327, 0, 6, 32, 20, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15329, 0, 6, 32, 20, 3, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15331, 0, 6, 32, 20, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15325, 0, 6, 32, 20, 3, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
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
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15326, 6, 0, 20, 32, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15328, 6, 0, 20, 32, 3, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15330, 6, 0, 20, 32, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15324, 6, 0, 20, 32, 3, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height + 8, TUNNEL_2);
			break;
		case 3:
			paint_util_push_tunnel_left(height + 8, TUNNEL_2);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15316, 0, 6, 32, 20, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15318, 0, 6, 32, 20, 3, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15320, 0, 6, 32, 20, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15322, 0, 6, 32, 20, 3, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
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
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15317, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15319, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15321, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(0, 4, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15323, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 8, TUNNEL_2);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 8, TUNNEL_2);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	lim_launched_rc_track_right_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	lim_launched_rc_track_left_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_twist_down_to_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15758, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15782, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15761, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15785, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15764, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15788, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15767, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15791, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15759, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15783, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15762, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15786, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15765, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15789, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15768, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15792, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15760, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15784, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15763, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15787, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15766, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15790, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15769, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15793, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		}
		switch (direction) {
		case 1:
			paint_util_push_tunnel_right(height - 32, TUNNEL_3);
			break;
		case 2:
			paint_util_push_tunnel_left(height - 32, TUNNEL_3);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_twist_down_to_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15770, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15794, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15773, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15797, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15776, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15800, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15779, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15803, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15771, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15795, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15774, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15798, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15777, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15801, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15780, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15804, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15772, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15796, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15775, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15799, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15778, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15802, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15781, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15805, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		}
		switch (direction) {
		case 1:
			paint_util_push_tunnel_right(height + 32, TUNNEL_3);
			break;
		case 2:
			paint_util_push_tunnel_left(height + 32, TUNNEL_3);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_twist_up_to_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15766, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15790, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15769, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15793, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15760, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15784, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15763, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15787, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 32, TUNNEL_3);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15765, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15789, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15768, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15792, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15759, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15783, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15762, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15786, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15764, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15788, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15767, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15791, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15758, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15782, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15761, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15785, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 1:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 2:
			paint_util_push_tunnel_left(height, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_twist_up_to_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15778, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15802, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15781, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15805, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15772, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15796, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15775, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15799, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 32, TUNNEL_3);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 64, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15777, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15801, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15780, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15804, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15771, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15795, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15774, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15798, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15776, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15800, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15779, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15803, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15770, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15794, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15773, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15797, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 1:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 2:
			paint_util_push_tunnel_left(height, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_corkscrew_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15734, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15737, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15740, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15743, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15735, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15738, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15741, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15744, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
			break;
		}
		paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15736, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15739, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15742, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15745, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height + 35, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height + 8, TUNNEL_0);
			break;
		case 3:
			paint_util_push_tunnel_left(height + 8, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_corkscrew_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15746, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15749, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15752, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15755, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15747, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15750, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15753, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15756, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
			break;
		}
		paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15748, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15751, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15754, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15757, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height + 35, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 8, TUNNEL_0);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 8, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_corkscrew_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_right_corkscrew_up(rideIndex, 2 - trackSequence, (direction + 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_corkscrew_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_left_corkscrew_up(rideIndex, 2 - trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_half_banked_helix_up_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15165, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15172, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15168, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15171, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15162, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15164, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15167, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15170, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15161, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15163, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15166, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15169, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15173, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15160, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height + 8, TUNNEL_0);
			break;
		case 3:
			paint_util_push_tunnel_left(height + 8, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15162, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15165, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15172, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15168, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15171, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 1:
			paint_util_push_tunnel_left(height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15161, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15164, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15167, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15170, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 7:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15160, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15163, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15166, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15169, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15173, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_half_banked_helix_up_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15146, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15149, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15152, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15155, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15159, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15147, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15150, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15153, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15156, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15148, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15151, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15158, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15154, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15157, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 8, TUNNEL_0);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 8, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 4:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15149, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15152, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15155, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15159, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15146, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15150, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15153, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15156, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15147, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 7:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15151, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15158, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15154, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15157, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15148, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_half_banked_helix_down_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (trackSequence >= 4) {
		trackSequence -= 4;
		direction = (direction - 1) & 3;
	}
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	lim_launched_rc_track_right_half_banked_helix_up_small(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_half_banked_helix_down_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (trackSequence >= 4) {
		trackSequence -= 4;
		direction = (direction + 1) & 3;
	}
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	lim_launched_rc_track_left_half_banked_helix_up_small(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_half_banked_helix_up_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15247, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15258, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15252, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15257, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15242, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15246, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15251, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15256, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15241, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15245, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15250, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15255, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15240, 0, 0, 16, 16, 3, height, 0, 0, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15244, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15249, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15254, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15239, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15243, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15248, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15253, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15259, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15238, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height + 8, TUNNEL_0);
			break;
		case 3:
			paint_util_push_tunnel_left(height + 8, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 7:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15242, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15247, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15258, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15252, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15257, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 1:
			paint_util_push_tunnel_left(height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15241, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15246, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15251, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15256, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 10:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15240, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15245, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15250, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15255, 0, 0, 16, 16, 3, height, 0, 16, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15239, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15244, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15249, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15254, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 13:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15238, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15243, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15248, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15253, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15259, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_half_banked_helix_up_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15216, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15221, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15226, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15231, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15237, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15217, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15222, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15227, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15232, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15218, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15223, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15228, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15233, 0, 0, 16, 16, 3, height, 0, 16, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15219, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15224, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15229, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15234, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15220, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15225, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15236, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15230, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15235, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 8, TUNNEL_0);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 8, TUNNEL_0);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 7:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15221, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15226, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15231, 0, 0, 20, 32, 3, height, 6, 0, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15237, 0, 0, 1, 32, 26, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15216, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height, TUNNEL_0);
			break;
		case 3:
			paint_util_push_tunnel_left(height, TUNNEL_0);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15222, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15227, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15232, 0, 0, 16, 32, 3, height, 0, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15217, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 10:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15223, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15228, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15233, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15218, 0, 0, 16, 16, 3, height, 0, 0, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15224, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15229, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15234, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15219, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 13:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15225, 0, 0, 32, 20, 3, height, 0, 6, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15236, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15230, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15235, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15220, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_half_banked_helix_down_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (trackSequence >= 7) {
		trackSequence -= 7;
		direction = (direction - 1) & 3;
	}
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	lim_launched_rc_track_right_half_banked_helix_up_large(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_half_banked_helix_down_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (trackSequence >= 7) {
		trackSequence -= 7;
		direction = (direction + 1) & 3;
	}
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	lim_launched_rc_track_left_half_banked_helix_up_large(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_quarter_turn_1_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15341, 0, 0, 28, 28, 3, height, 2, 2, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15345, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15342, 0, 0, 28, 28, 3, height, 2, 2, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15346, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15343, 0, 0, 28, 28, 3, height, 2, 2, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15347, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15340, 0, 0, 28, 28, 3, height, 2, 2, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15344, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
		break;
	}
	switch (direction) {
	case 0:
		paint_util_push_tunnel_left(height - 8, TUNNEL_2);
		break;
	case 2:
		paint_util_push_tunnel_right(height + 56, TUNNEL_1);
		break;
	case 3:
		paint_util_push_tunnel_right(height - 8, TUNNEL_1);
		paint_util_push_tunnel_left(height + 56, TUNNEL_1);
		break;
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 104, 0x20);
}

static void lim_launched_rc_track_right_quarter_turn_1_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15332, 0, 0, 28, 28, 3, height, 2, 2, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15336, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15333, 0, 0, 28, 28, 3, height, 2, 2, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15337, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15334, 0, 0, 28, 28, 3, height, 2, 2, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15338, 0, 0, 2, 28, 59, height, 28, 2, height + 2);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15335, 0, 0, 28, 28, 3, height, 2, 2, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15339, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
		break;
	}
	switch (direction) {
	case 0:
		paint_util_push_tunnel_right(height + 56, TUNNEL_1);
		paint_util_push_tunnel_left(height - 8, TUNNEL_1);
		break;
	case 1:
		paint_util_push_tunnel_left(height + 56, TUNNEL_2);
		break;
	case 3:
		paint_util_push_tunnel_right(height - 8, TUNNEL_1);
		break;
	}
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 104, 0x20);
}

static void lim_launched_rc_track_left_quarter_turn_1_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_right_quarter_turn_1_60_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_quarter_turn_1_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_left_quarter_turn_1_60_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15018, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15019, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void lim_launched_rc_track_25_deg_up_left_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15594, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15595, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15596, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15597, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void lim_launched_rc_track_25_deg_up_right_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15598, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15599, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15600, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15601, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void lim_launched_rc_track_on_ride_photo(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 22432, 0, 0, 32, 32, 1, height);
		metal_a_supports_paint_setup(0, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		metal_a_supports_paint_setup(0, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15004, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
		sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 25623, 26, 0, 1, 1, 19, height + 3);
		break;
	case 1:
		sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 22432, 0, 0, 32, 32, 1, height);
		metal_a_supports_paint_setup(0, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		metal_a_supports_paint_setup(0, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15005, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
		sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 25624, 6, 0, 1, 1, 19, height + 3);
		break;
	case 2:
		sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 22432, 0, 0, 32, 32, 1, height);
		metal_a_supports_paint_setup(0, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		metal_a_supports_paint_setup(0, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15004, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
		sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 25625, 6, 0, 1, 1, 19, height + 3);
		break;
	case 3:
		sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 22432, 0, 0, 32, 32, 1, height);
		metal_a_supports_paint_setup(0, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		metal_a_supports_paint_setup(0, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15005, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
		sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 25626, 26, 0, 1, 1, 19, height + 3);
		break;
	}
	sub_98196C_rotated(direction, 0x00000000, 0, 0, 0, 0, 0, height - 48);
	sub_98196C_rotated(direction, 0x00000000, 0, 0, 0, 0, 0, height - 48);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void lim_launched_rc_track_25_deg_down_left_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_25_deg_up_right_banked(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_25_deg_down_right_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_25_deg_up_left_banked(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_90_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15702, 0, 0, 2, 20, 31, height, 4, 6, height + 8);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15703, 0, 0, 2, 20, 31, height, 24, 6, height + 8);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15704, 0, 0, 2, 20, 31, height, 24, 6, height + 8);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15705, 0, 0, 2, 20, 31, height, 4, 6, height + 8);
			break;
		}
		paint_util_set_vertical_tunnel(height + 32);
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		break;
	}
}

static void lim_launched_rc_track_90_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_90_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_60_deg_up_to_90_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15694, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15695, 0, 0, 2, 20, 55, height, 24, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15696, 0, 0, 2, 20, 55, height, 24, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15697, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
		}
		paint_util_set_vertical_tunnel(height + 48);
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		break;
	}
}

static void lim_launched_rc_track_90_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_60_deg_up_to_90_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_90_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15698, 0, 0, 6, 20, 3, height, 0, 6, height + 2);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15699, 0, 0, 2, 20, 31, height, 39, 6, height + 8);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15700, 0, 0, 2, 20, 31, height, 39, 6, height + 8);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15701, 0, 0, 6, 20, 3, height, 0, 6, height + 2);
		break;
	}
	switch (direction) {
	case 1:
		paint_util_push_tunnel_right(height + 48, TUNNEL_2);
		break;
	case 2:
		paint_util_push_tunnel_left(height + 48, TUNNEL_2);
		break;
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 80, 0x20);
}

static void lim_launched_rc_track_60_deg_down_to_90_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15700, 0, 0, 2, 20, 31, height, 39, 6, height + 8);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15701, 0, 0, 6, 20, 3, height, 0, 6, height + 2);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15698, 0, 0, 6, 20, 3, height, 0, 6, height + 2);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15699, 0, 0, 2, 20, 31, height, 39, 6, height + 8);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 48, TUNNEL_2);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 80, 0x20);
		break;
	case 1:
		break;
	}
}

static void lim_launched_rc_track_left_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15526, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15530, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15534, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15538, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15527, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15531, 0, 0, 34, 16, 3, height, 0, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15535, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15539, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15528, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15532, 0, 0, 16, 16, 3, height, 16, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15536, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15540, 0, 0, 16, 16, 3, height, 0, 0, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15529, 0, 0, 16, 16, 3, height, 16, 16, height);
			metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15533, 0, 0, 16, 18, 3, height, 0, 16, height);
			metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15537, 0, 0, 16, 16, 3, height, 0, 0, height);
			metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15541, 0, 0, 16, 16, 3, height, 16, 0, height);
			metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15510, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15514, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15518, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15522, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15511, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15515, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15519, 0, 0, 34, 16, 3, height, 0, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15523, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15512, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15516, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15520, 0, 0, 28, 28, 3, height, 4, 4, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15524, 0, 0, 16, 16, 3, height, 0, 16, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15513, 0, 0, 16, 16, 3, height, 16, 0, height);
			metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15517, 0, 0, 16, 16, 3, height, 0, 0, height);
			metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15521, 0, 0, 16, 18, 3, height, 0, 16, height);
			metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15525, 0, 0, 16, 16, 3, height, 16, 16, height);
			metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
	lim_launched_rc_track_right_eighth_to_diag(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
	lim_launched_rc_track_left_eighth_to_diag(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15558, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15562, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15566, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15570, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15559, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15563, 0, 0, 34, 16, 0, height, 0, 0, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15567, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15571, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15560, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15564, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15568, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15572, 0, 0, 16, 16, 3, height, 0, 0, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15561, 0, 0, 16, 16, 3, height, 16, 16, height);
			metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15565, 0, 0, 16, 18, 0, height, 0, 16, height + 27);
			metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15569, 0, 0, 16, 16, 3, height, 0, 0, height);
			metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15573, 0, 0, 16, 16, 3, height, 16, 0, height);
			metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15542, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15546, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15550, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15554, 0, 0, 32, 1, 26, height, 0, 27, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15543, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15547, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15551, 0, 0, 34, 16, 0, height, 0, 0, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15555, 0, 0, 32, 16, 3, height, 0, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15544, 0, 0, 16, 16, 3, height, 0, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15548, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15552, 0, 0, 28, 28, 0, height, 4, 4, height + 27);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15556, 0, 0, 16, 16, 3, height, 0, 16, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15545, 0, 0, 16, 16, 3, height, 16, 0, height);
			metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15549, 0, 0, 16, 16, 3, height, 0, 0, height);
			metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15553, 0, 0, 16, 18, 0, height, 0, 16, height + 27);
			metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15557, 0, 0, 16, 16, 3, height, 16, 16, height);
			metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
	lim_launched_rc_track_right_eighth_bank_to_diag(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
	lim_launched_rc_track_left_eighth_bank_to_diag(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

static void lim_launched_rc_track_diag_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15451, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15423, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15448, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15420, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15450, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15422, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15449, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15421, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15463, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15435, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15460, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15432, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15462, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15434, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15461, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15433, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15475, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15447, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 104, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15472, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15444, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 104, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15474, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15446, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 104, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 36, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15473, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 36, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 36, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 36, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 36, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15445, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 36, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 36, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 36, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 104, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15455, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15427, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15452, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15424, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15454, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15426, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15453, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15425, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15467, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15439, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15464, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15436, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15466, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15438, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 16, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15465, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 16, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 16, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 16, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 16, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15437, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 16, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 16, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 16, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15471, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15443, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15468, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15440, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15470, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15442, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 21, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15469, -16, -16, 16, 16, 3, height, 0, 0, height);
				metal_b_supports_paint_setup(0, 0, 21, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 21, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 21, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 21, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15441, -16, -16, 16, 16, 3, height, 0, 0, height);
				metal_b_supports_paint_setup(0, 0, 21, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 21, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 21, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15459, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15431, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15456, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15428, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15458, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15430, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15457, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15429, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15461, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15433, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15462, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15434, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15460, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15432, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15463, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15435, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15473, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15445, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 104, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15474, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15446, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 104, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15472, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15444, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 104, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 28, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15475, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 28, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 28, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 28, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 28, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15447, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 28, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 28, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 28, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 104, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15457, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15429, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15458, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15430, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15456, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15428, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15459, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15431, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15469, -16, -16, 16, 16, 3, height, 0, 0, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15441, -16, -16, 16, 16, 3, height, 0, 0, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15470, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15442, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15468, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15440, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 17, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15471, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 17, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 17, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 17, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 17, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15443, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 17, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 17, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 17, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15465, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15437, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15466, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15438, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15464, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15436, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15467, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15439, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15453, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 3:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15425, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15454, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15426, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15452, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		} else {
			switch (direction) {
			case 2:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15424, -16, -16, 32, 32, 3, height, -16, -16, height);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		if (track_element_is_lift_hill(mapElement)) {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15455, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		} else {
			switch (direction) {
			case 0:
				metal_b_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 1:
				sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15427, -16, -16, 32, 32, 3, height, -16, -16, height);
				metal_b_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 2:
				metal_b_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			case 3:
				metal_b_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
				break;
			}
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15503, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15500, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15504, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15502, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15501, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15508, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15505, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15507, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15509, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15506, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15506, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15507, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15509, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15505, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15508, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15501, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15502, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15500, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15504, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15503, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15493, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15490, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15494, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15492, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15491, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15498, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15495, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15497, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15499, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15496, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15483, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15480, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15484, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15482, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(0, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15481, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(0, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(0, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(0, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15488, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15485, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15487, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15489, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(0, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15486, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(0, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(0, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(0, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15486, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15487, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15489, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15485, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(0, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15488, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(0, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(0, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(0, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15481, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15482, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15480, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15484, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(0, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15483, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(0, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(0, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(0, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 56, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15496, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15497, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15499, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15495, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15498, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15491, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15492, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15490, -16, -16, 32, 32, 3, height, -16, -16, height);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15494, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_b_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15493, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_b_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_b_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_b_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15479, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15476, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15478, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15477, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_diag_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15477, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15478, -16, -16, 32, 32, 3, height, -16, -16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 2:
		switch (direction) {
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15476, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			metal_a_supports_paint_setup(0, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15479, -16, -16, 32, 32, 3, height, -16, -16, height);
			metal_a_supports_paint_setup(0, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(0, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(0, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 32, 0x20);
		break;
	}
}

static void lim_launched_rc_track_90_deg_to_inverted_flat_quarter_loop_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15722, 0, 0, 2, 20, 31, height, 4, 6, height + 8);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15725, 0, 0, 2, 20, 31, height, 24, 6, height + 8);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15728, 0, 0, 2, 20, 31, height, 24, 6, height + 8);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15731, 0, 0, 2, 20, 31, height, 4, 6, height + 8);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 88, 0x20);
		break;
	case 1:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15723, 0, 0, 2, 20, 31, height, -8, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15726, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15729, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15732, 0, 0, 2, 20, 31, height, -8, 6, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 64, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15724, 0, 0, 32, 20, 3, height, 0, 6, height + 24);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15727, 0, 0, 2, 20, 31, height, 24, 6, height + 8);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15730, 0, 0, 2, 20, 31, height, 24, 6, height + 8);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15733, 0, 0, 32, 20, 3, height, 0, 6, height + 24);
			break;
		}
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height + 16, TUNNEL_0);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 48, 0x20);
		break;
	}
}

static void lim_launched_rc_track_inverted_flat_to_90_deg_quarter_loop_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_90_deg_to_inverted_flat_quarter_loop_up(rideIndex, 2 - trackSequence, direction, height, mapElement);
}

static void lim_launched_rc_track_block_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15020, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15021, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

static void lim_launched_rc_track_left_banked_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15689, 0, 6, 32, 20, 3, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15691, 0, 6, 32, 1, 34, height, 0, 27, height);
			break;
		case 2:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15693, 0, 6, 32, 20, 3, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15687, 0, 6, 32, 20, 3, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
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
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15688, 6, 0, 20, 32, 3, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15690, 6, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15692, 6, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15686, 6, 0, 20, 32, 3, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height + 8, TUNNEL_2);
			break;
		case 3:
			paint_util_push_tunnel_left(height + 8, TUNNEL_2);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_banked_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15678, 0, 6, 32, 20, 3, height);
			break;
		case 1:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15680, 0, 6, 32, 20, 3, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15682, 0, 6, 32, 1, 34, height, 0, 27, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15684, 0, 6, 32, 20, 3, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
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
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15679, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15681, 6, 0, 1, 32, 34, height, 27, 0, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15683, 6, 0, 1, 32, 34, height, 27, 0, height);
			metal_a_supports_paint_setup(0, 4, 10, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15685, 6, 0, 20, 32, 3, height);
			metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		}
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 8, TUNNEL_2);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 8, TUNNEL_2);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_banked_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	lim_launched_rc_track_right_banked_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_banked_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	lim_launched_rc_track_left_banked_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_banked_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15658, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15663, 0, 0, 32, 1, 34, height, 0, 27, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15668, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15673, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15659, 0, 0, 32, 16, 3, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15664, 0, 0, 1, 1, 34, height, 30, 30, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15669, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15674, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15660, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15665, 0, 0, 1, 1, 34, height, 30, 30, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15670, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15675, 0, 0, 16, 16, 3, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 64, 0x20);
		break;
	case 4:
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 5:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15661, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15666, 0, 0, 1, 1, 34, height, 30, 30, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15671, 0, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15676, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15662, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15667, 0, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15672, 0, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15677, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 2:
			paint_util_push_tunnel_right(height + 8, TUNNEL_2);
			break;
		case 3:
			paint_util_push_tunnel_left(height + 8, TUNNEL_2);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_right_banked_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15638, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15643, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15648, 0, 0, 32, 1, 34, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15653, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 1:
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 2:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15639, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15644, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15649, 0, 0, 1, 1, 34, height, 30, 30, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15654, 0, 0, 32, 16, 3, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15640, 0, 0, 16, 16, 3, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15645, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15650, 0, 0, 1, 1, 34, height, 30, 30, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15655, 0, 0, 16, 16, 3, height, 0, 16, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 64, 0x20);
		break;
	case 4:
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 5:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15641, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15646, 0, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15651, 0, 0, 1, 1, 34, height, 30, 30, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15656, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15642, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15647, 0, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15652, 0, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15657, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 8, TUNNEL_2);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 8, TUNNEL_2);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

static void lim_launched_rc_track_left_banked_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	lim_launched_rc_track_right_banked_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_banked_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	lim_launched_rc_track_left_banked_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_25_deg_up_to_left_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15602, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15603, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15610, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15604, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15605, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void lim_launched_rc_track_25_deg_up_to_right_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15606, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15607, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15608, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15611, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15609, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void lim_launched_rc_track_left_banked_25_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15612, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15613, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15620, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15614, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15615, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void lim_launched_rc_track_right_banked_25_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15616, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15617, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15618, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15621, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15619, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

static void lim_launched_rc_track_25_deg_down_to_left_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_right_banked_25_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_25_deg_down_to_right_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_left_banked_25_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_banked_25_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_25_deg_up_to_right_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_banked_25_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_25_deg_up_to_left_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_banked_flat_to_left_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15622, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15623, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15624, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15625, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void lim_launched_rc_track_right_banked_flat_to_right_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15626, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15627, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15628, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15629, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void lim_launched_rc_track_left_banked_25_deg_up_to_left_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15630, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15631, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15632, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15633, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void lim_launched_rc_track_right_banked_25_deg_up_to_right_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15634, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15635, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15636, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15637, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void lim_launched_rc_track_left_banked_flat_to_left_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_right_banked_25_deg_up_to_right_banked_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_banked_flat_to_right_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_left_banked_25_deg_up_to_left_banked_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_banked_25_deg_down_to_left_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_right_banked_flat_to_right_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_banked_25_deg_down_to_right_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_left_banked_flat_to_left_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_flat_to_left_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15574, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15575, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15582, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15576, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15577, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void lim_launched_rc_track_flat_to_right_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15578, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15579, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15580, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15583, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15581, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

static void lim_launched_rc_track_left_banked_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15584, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15585, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15592, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15586, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15587, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void lim_launched_rc_track_right_banked_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15588, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15589, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15590, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15593, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15591, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(0, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

static void lim_launched_rc_track_flat_to_left_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_right_banked_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_flat_to_right_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_left_banked_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_banked_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_flat_to_right_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_banked_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_flat_to_left_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void lim_launched_rc_track_left_quarter_turn_1_90_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15706, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15707, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15715, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15716, 0, 0, 2, 2, 63, height, 24, 24, height + 8);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15709, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15717, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		}
		paint_util_set_vertical_tunnel(height + 96);
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 96, 0x20);
		break;
	case 1:
		break;
	}
}

static void lim_launched_rc_track_right_quarter_turn_1_90_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15710, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15718, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15719, 0, 0, 2, 2, 63, height, 24, 24, height + 8);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15712, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15720, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15713, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			break;
		}
		paint_util_set_vertical_tunnel(height + 96);
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 96, 0x20);
		break;
	case 1:
		break;
	}
}

static void lim_launched_rc_track_left_quarter_turn_1_90_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_right_quarter_turn_1_90_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

static void lim_launched_rc_track_right_quarter_turn_1_90_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	lim_launched_rc_track_left_quarter_turn_1_90_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

TRACK_PAINT_FUNCTION get_track_paint_function_lim_launched_rc(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT:
		return lim_launched_rc_track_flat;
	case TRACK_ELEM_25_DEG_UP:
		return lim_launched_rc_track_25_deg_up;
	case TRACK_ELEM_60_DEG_UP:
		return lim_launched_rc_track_60_deg_up;
	case TRACK_ELEM_FLAT_TO_25_DEG_UP:
		return lim_launched_rc_track_flat_to_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
		return lim_launched_rc_track_25_deg_up_to_60_deg_up;
	case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
		return lim_launched_rc_track_60_deg_up_to_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_FLAT:
		return lim_launched_rc_track_25_deg_up_to_flat;
	case TRACK_ELEM_25_DEG_DOWN:
		return lim_launched_rc_track_25_deg_down;
	case TRACK_ELEM_60_DEG_DOWN:
		return lim_launched_rc_track_60_deg_down;
	case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
		return lim_launched_rc_track_flat_to_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
		return lim_launched_rc_track_25_deg_down_to_60_deg_down;
	case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
		return lim_launched_rc_track_60_deg_down_to_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
		return lim_launched_rc_track_25_deg_down_to_flat;
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
		return lim_launched_rc_track_left_quarter_turn_5;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
		return lim_launched_rc_track_right_quarter_turn_5;
	case TRACK_ELEM_FLAT_TO_LEFT_BANK:
		return lim_launched_rc_track_flat_to_left_bank;
	case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
		return lim_launched_rc_track_flat_to_right_bank;
	case TRACK_ELEM_LEFT_BANK_TO_FLAT:
		return lim_launched_rc_track_left_bank_to_flat;
	case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
		return lim_launched_rc_track_right_bank_to_flat;
	case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
		return lim_launched_rc_track_banked_left_quarter_turn_5;
	case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
		return lim_launched_rc_track_banked_right_quarter_turn_5;
	case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
		return lim_launched_rc_track_left_bank_to_25_deg_up;
	case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
		return lim_launched_rc_track_right_bank_to_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
		return lim_launched_rc_track_25_deg_up_to_left_bank;
	case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
		return lim_launched_rc_track_25_deg_up_to_right_bank;
	case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
		return lim_launched_rc_track_left_bank_to_25_deg_down;
	case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
		return lim_launched_rc_track_right_bank_to_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
		return lim_launched_rc_track_25_deg_down_to_left_bank;
	case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
		return lim_launched_rc_track_25_deg_down_to_right_bank;
	case TRACK_ELEM_LEFT_BANK:
		return lim_launched_rc_track_left_bank;
	case TRACK_ELEM_RIGHT_BANK:
		return lim_launched_rc_track_right_bank;
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
		return lim_launched_rc_track_left_quarter_turn_5_25_deg_up;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
		return lim_launched_rc_track_right_quarter_turn_5_25_deg_up;
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
		return lim_launched_rc_track_left_quarter_turn_5_25_deg_down;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
		return lim_launched_rc_track_right_quarter_turn_5_25_deg_down;
	case TRACK_ELEM_S_BEND_LEFT:
		return lim_launched_rc_track_s_bend_left;
	case TRACK_ELEM_S_BEND_RIGHT:
		return lim_launched_rc_track_s_bend_right;
	case TRACK_ELEM_LEFT_VERTICAL_LOOP:
		return lim_launched_rc_track_left_vertical_loop;
	case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
		return lim_launched_rc_track_right_vertical_loop;
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
		return lim_launched_rc_track_left_quarter_turn_3;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
		return lim_launched_rc_track_right_quarter_turn_3;
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
		return lim_launched_rc_track_left_quarter_turn_3_bank;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
		return lim_launched_rc_track_right_quarter_turn_3_bank;
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
		return lim_launched_rc_track_left_quarter_turn_3_25_deg_up;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
		return lim_launched_rc_track_right_quarter_turn_3_25_deg_up;
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
		return lim_launched_rc_track_left_quarter_turn_3_25_deg_down;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
		return lim_launched_rc_track_right_quarter_turn_3_25_deg_down;
	case TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP:
		return lim_launched_rc_track_left_twist_down_to_up;
	case TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP:
		return lim_launched_rc_track_right_twist_down_to_up;
	case TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN:
		return lim_launched_rc_track_left_twist_up_to_down;
	case TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN:
		return lim_launched_rc_track_right_twist_up_to_down;
	case TRACK_ELEM_LEFT_CORKSCREW_UP:
		return lim_launched_rc_track_left_corkscrew_up;
	case TRACK_ELEM_RIGHT_CORKSCREW_UP:
		return lim_launched_rc_track_right_corkscrew_up;
	case TRACK_ELEM_LEFT_CORKSCREW_DOWN:
		return lim_launched_rc_track_left_corkscrew_down;
	case TRACK_ELEM_RIGHT_CORKSCREW_DOWN:
		return lim_launched_rc_track_right_corkscrew_down;
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
		return lim_launched_rc_track_left_half_banked_helix_up_small;
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
		return lim_launched_rc_track_right_half_banked_helix_up_small;
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
		return lim_launched_rc_track_left_half_banked_helix_down_small;
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
		return lim_launched_rc_track_right_half_banked_helix_down_small;
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
		return lim_launched_rc_track_left_half_banked_helix_up_large;
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
		return lim_launched_rc_track_right_half_banked_helix_up_large;
	case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
		return lim_launched_rc_track_left_half_banked_helix_down_large;
	case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
		return lim_launched_rc_track_right_half_banked_helix_down_large;
	case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP:
		return lim_launched_rc_track_left_quarter_turn_1_60_deg_up;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP:
		return lim_launched_rc_track_right_quarter_turn_1_60_deg_up;
	case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
		return lim_launched_rc_track_left_quarter_turn_1_60_deg_down;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
		return lim_launched_rc_track_right_quarter_turn_1_60_deg_down;
	case TRACK_ELEM_BRAKES:
		return lim_launched_rc_track_brakes;
	case TRACK_ELEM_25_DEG_UP_LEFT_BANKED:
		return lim_launched_rc_track_25_deg_up_left_banked;
	case TRACK_ELEM_25_DEG_UP_RIGHT_BANKED:
		return lim_launched_rc_track_25_deg_up_right_banked;
	case TRACK_ELEM_ON_RIDE_PHOTO:
		return lim_launched_rc_track_on_ride_photo;
	case TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED:
		return lim_launched_rc_track_25_deg_down_left_banked;
	case TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED:
		return lim_launched_rc_track_25_deg_down_right_banked;
	case TRACK_ELEM_90_DEG_UP:
		return lim_launched_rc_track_90_deg_up;
	case TRACK_ELEM_90_DEG_DOWN:
		return lim_launched_rc_track_90_deg_down;
	case TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP:
		return lim_launched_rc_track_60_deg_up_to_90_deg_up;
	case TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN:
		return lim_launched_rc_track_90_deg_down_to_60_deg_down;
	case TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP:
		return lim_launched_rc_track_90_deg_up_to_60_deg_up;
	case TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN:
		return lim_launched_rc_track_60_deg_down_to_90_deg_down;
	case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
		return lim_launched_rc_track_left_eighth_to_diag;
	case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
		return lim_launched_rc_track_right_eighth_to_diag;
	case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
		return lim_launched_rc_track_left_eighth_to_orthogonal;
	case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
		return lim_launched_rc_track_right_eighth_to_orthogonal;
	case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
		return lim_launched_rc_track_left_eighth_bank_to_diag;
	case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
		return lim_launched_rc_track_right_eighth_bank_to_diag;
	case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
		return lim_launched_rc_track_left_eighth_bank_to_orthogonal;
	case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
		return lim_launched_rc_track_right_eighth_bank_to_orthogonal;
	case TRACK_ELEM_DIAG_FLAT:
		return lim_launched_rc_track_diag_flat;
	case TRACK_ELEM_DIAG_25_DEG_UP:
		return lim_launched_rc_track_diag_25_deg_up;
	case TRACK_ELEM_DIAG_60_DEG_UP:
		return lim_launched_rc_track_diag_60_deg_up;
	case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
		return lim_launched_rc_track_diag_flat_to_25_deg_up;
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
		return lim_launched_rc_track_diag_25_deg_up_to_60_deg_up;
	case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
		return lim_launched_rc_track_diag_60_deg_up_to_25_deg_up;
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
		return lim_launched_rc_track_diag_25_deg_up_to_flat;
	case TRACK_ELEM_DIAG_25_DEG_DOWN:
		return lim_launched_rc_track_diag_25_deg_down;
	case TRACK_ELEM_DIAG_60_DEG_DOWN:
		return lim_launched_rc_track_diag_60_deg_down;
	case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
		return lim_launched_rc_track_diag_flat_to_25_deg_down;
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
		return lim_launched_rc_track_diag_25_deg_down_to_60_deg_down;
	case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
		return lim_launched_rc_track_diag_60_deg_down_to_25_deg_down;
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
		return lim_launched_rc_track_diag_25_deg_down_to_flat;
	case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
		return lim_launched_rc_track_diag_flat_to_left_bank;
	case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
		return lim_launched_rc_track_diag_flat_to_right_bank;
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
		return lim_launched_rc_track_diag_left_bank_to_flat;
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
		return lim_launched_rc_track_diag_right_bank_to_flat;
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
		return lim_launched_rc_track_diag_left_bank_to_25_deg_up;
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
		return lim_launched_rc_track_diag_right_bank_to_25_deg_up;
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
		return lim_launched_rc_track_diag_25_deg_up_to_left_bank;
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
		return lim_launched_rc_track_diag_25_deg_up_to_right_bank;
	case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
		return lim_launched_rc_track_diag_left_bank_to_25_deg_down;
	case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
		return lim_launched_rc_track_diag_right_bank_to_25_deg_down;
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
		return lim_launched_rc_track_diag_25_deg_down_to_left_bank;
	case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
		return lim_launched_rc_track_diag_25_deg_down_to_right_bank;
	case TRACK_ELEM_DIAG_LEFT_BANK:
		return lim_launched_rc_track_diag_left_bank;
	case TRACK_ELEM_DIAG_RIGHT_BANK:
		return lim_launched_rc_track_diag_right_bank;
	case TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP:
		return lim_launched_rc_track_90_deg_to_inverted_flat_quarter_loop_up;
	case TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN:
		return lim_launched_rc_track_inverted_flat_to_90_deg_quarter_loop_down;
	case TRACK_ELEM_BLOCK_BRAKES:
		return lim_launched_rc_track_block_brakes;
	case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
		return lim_launched_rc_track_left_banked_quarter_turn_3_25_deg_up;
	case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
		return lim_launched_rc_track_right_banked_quarter_turn_3_25_deg_up;
	case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
		return lim_launched_rc_track_left_banked_quarter_turn_3_25_deg_down;
	case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
		return lim_launched_rc_track_right_banked_quarter_turn_3_25_deg_down;
	case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
		return lim_launched_rc_track_left_banked_quarter_turn_5_25_deg_up;
	case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
		return lim_launched_rc_track_right_banked_quarter_turn_5_25_deg_up;
	case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
		return lim_launched_rc_track_left_banked_quarter_turn_5_25_deg_down;
	case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
		return lim_launched_rc_track_right_banked_quarter_turn_5_25_deg_down;
	case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP:
		return lim_launched_rc_track_25_deg_up_to_left_banked_25_deg_up;
	case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP:
		return lim_launched_rc_track_25_deg_up_to_right_banked_25_deg_up;
	case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP:
		return lim_launched_rc_track_left_banked_25_deg_up_to_25_deg_up;
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP:
		return lim_launched_rc_track_right_banked_25_deg_up_to_25_deg_up;
	case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN:
		return lim_launched_rc_track_25_deg_down_to_left_banked_25_deg_down;
	case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN:
		return lim_launched_rc_track_25_deg_down_to_right_banked_25_deg_down;
	case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
		return lim_launched_rc_track_left_banked_25_deg_down_to_25_deg_down;
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
		return lim_launched_rc_track_right_banked_25_deg_down_to_25_deg_down;
	case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP:
		return lim_launched_rc_track_left_banked_flat_to_left_banked_25_deg_up;
	case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
		return lim_launched_rc_track_right_banked_flat_to_right_banked_25_deg_up;
	case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT:
		return lim_launched_rc_track_left_banked_25_deg_up_to_left_banked_flat;
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT:
		return lim_launched_rc_track_right_banked_25_deg_up_to_right_banked_flat;
	case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
		return lim_launched_rc_track_left_banked_flat_to_left_banked_25_deg_down;
	case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
		return lim_launched_rc_track_right_banked_flat_to_right_banked_25_deg_down;
	case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT:
		return lim_launched_rc_track_left_banked_25_deg_down_to_left_banked_flat;
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT:
		return lim_launched_rc_track_right_banked_25_deg_down_to_right_banked_flat;
	case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP:
		return lim_launched_rc_track_flat_to_left_banked_25_deg_up;
	case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
		return lim_launched_rc_track_flat_to_right_banked_25_deg_up;
	case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT:
		return lim_launched_rc_track_left_banked_25_deg_up_to_flat;
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT:
		return lim_launched_rc_track_right_banked_25_deg_up_to_flat;
	case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
		return lim_launched_rc_track_flat_to_left_banked_25_deg_down;
	case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
		return lim_launched_rc_track_flat_to_right_banked_25_deg_down;
	case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT:
		return lim_launched_rc_track_left_banked_25_deg_down_to_flat;
	case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT:
		return lim_launched_rc_track_right_banked_25_deg_down_to_flat;
	case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP:
		return lim_launched_rc_track_left_quarter_turn_1_90_deg_up;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP:
		return lim_launched_rc_track_right_quarter_turn_1_90_deg_up;
	case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
		return lim_launched_rc_track_left_quarter_turn_1_90_deg_down;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
		return lim_launched_rc_track_right_quarter_turn_1_90_deg_down;
	}
	return NULL;
}
