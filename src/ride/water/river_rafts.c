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
	SPR_RIVER_RAFTS_FLAT_SW_NE			= 20820,
	SPR_RIVER_RAFTS_FLAT_NW_SE			= 20821,
	SPR_RIVER_RAFTS_FLAT_NE_SW			= 20822,
	SPR_RIVER_RAFTS_FLAT_SE_NW			= 20823,
	SPR_RIVER_RAFTS_FLAT_FRONT_SW_NE	= 20824,
	SPR_RIVER_RAFTS_FLAT_FRONT_NW_SE	= 20825,
	SPR_RIVER_RAFTS_FLAT_FRONT_NE_SW	= 20826,
	SPR_RIVER_RAFTS_FLAT_FRONT_SE_NW	= 20827,
};

/** rct2: 0x0089B170 */
static void paint_river_rafts_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;

	if (direction & 1) {
		imageId = (direction == 1 ? SPR_RIVER_RAFTS_FLAT_NW_SE : SPR_RIVER_RAFTS_FLAT_SE_NW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 20, 32, 2, height, 6, 0, height, get_current_rotation());

		imageId = (direction == 1 ? SPR_RIVER_RAFTS_FLAT_FRONT_NW_SE : SPR_RIVER_RAFTS_FLAT_FRONT_SE_NW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 1, 32, 26, height, 27, 0, height, get_current_rotation());
	} else {
		imageId = (direction == 0 ? SPR_RIVER_RAFTS_FLAT_SW_NE : SPR_RIVER_RAFTS_FLAT_NE_SW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 20, 2, height, 0, 6, height, get_current_rotation());

		imageId = (direction == 0 ? SPR_RIVER_RAFTS_FLAT_FRONT_SW_NE : SPR_RIVER_RAFTS_FLAT_FRONT_NE_SW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 1, 26, height, 0, 27, height, get_current_rotation());
	}

	wooden_a_supports_paint_setup((direction & 1), 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);

	if (direction & 1) {
		paint_util_push_tunnel_right(height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_left(height, TUNNEL_6);
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0089B1A0 */
static void paint_river_rafts_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_ride *ride = get_ride(rideIndex);

	if (direction & 1) {
		uint32 imageId = (direction == 1 ? SPR_RIVER_RAFTS_FLAT_NW_SE : SPR_RIVER_RAFTS_FLAT_SE_NW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 20, 32, 1, height, 6, 0, height + 3, get_current_rotation());

		imageId = SPR_STATION_BASE_B_NW_SE | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());
	} else {
		uint32 imageId = (direction == 0 ? SPR_RIVER_RAFTS_FLAT_SW_NE : SPR_RIVER_RAFTS_FLAT_NE_SW) | RCT2_GLOBAL(0x00F44198, uint32);
		sub_98197C(imageId, 0, 0, 32, 20, 1, height, 0, 6, height + 3, get_current_rotation());

		imageId = SPR_STATION_BASE_B_SW_NE | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());
	}

	track_paint_util_draw_station_platform(ride, direction, height, 7, mapElement);
	
	wooden_a_supports_paint_setup((direction & 1), 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);

	if (direction & 1) {
		paint_util_push_tunnel_right(height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_left(height, TUNNEL_6);
	}

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0089B1D0 */
static void paint_river_rafts_track_left_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 spriteIds[4][5] = {
		{
			20937,
			20936,
			20935,
			20934,
			20933,
		},
		{
			20942,
			20941,
			20940,
			20939,
			20938,
		},
		{
			20947,
			20946,
			20945,
			20944,
			20943,
		},
		{
			20932,
			20931,
			20930,
			20929,
			20928,
		}
	};
	static const uint32 sideSpriteIds[4][5] = {
		{
			20937 - 20,
			20936 - 20,
			20935 - 20,
			20934 - 20,
			20933 - 20,
		},
		{
			20942 - 20,
			20941 - 20,
			20940 - 20,
			20939 - 20,
			20938 - 20,
		},
		{
			20947 - 20,
			20946 - 20,
			20945 - 20,
			20944 - 20,
			20943 - 20,
		},
		{
			20932 - 20,
			20931 - 20,
			20930 - 20,
			20929 - 20,
			20928 - 20,
		}
	};
	track_paint_util_right_quarter_turn_5_tiles_paint(2, height, direction, trackSequence, RCT2_GLOBAL(0x00F44198, uint32), spriteIds, get_current_rotation());
	track_paint_util_right_quarter_turn_5_tiles_paint(0, height, direction, trackSequence, RCT2_GLOBAL(0x00F44198, uint32), sideSpriteIds, get_current_rotation());

	if (trackSequence != 1 && trackSequence != 4) {
		static const uint8 supportTypes[][7] = {
			{ 0, 0xFF, 5, 3, 0xFF, 5, 1 },
			{ 1, 0xFF, 2, 4, 0xFF, 2, 0 },
			{ 0, 0xFF, 3, 5, 0xFF, 3, 1 },
			{ 1, 0xFF, 4, 2, 0xFF, 4, 0 },
		};
		uint8 supportType = supportTypes[direction][trackSequence];
		wooden_a_supports_paint_setup(supportType, 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
	}

	switch (trackSequence) {
	case 0: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
	case 1: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0); break;
	case 2: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4 | SEGMENT_B8 | SEGMENT_D0, direction), 0xFFFF, 0); break;
	case 3: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_CC | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0); break;
	case 4: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0); break;
	case 5: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_B8, direction), 0xFFFF, 0); break;
	case 6: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
	}

	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0089B1D0 */
static void paint_river_rafts_track_right_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 spriteIds[4][5] = {
		{
			20888,
			20889,
			20890,
			20891,
			20892,
		},
		{
			20893,
			20894,
			20895,
			20896,
			20897,
		},
		{
			20898,
			20899,
			20900,
			20901,
			20902,
		},
		{
			20903,
			20904,
			20905,
			20906,
			20907,
		}
	};
	static const uint32 sideSpriteIds[4][5] = {
		{
			20888 + 20,
			20889 + 20,
			20890 + 20,
			20891 + 20,
			20892 + 20,
		},
		{
			20893 + 20,
			20894 + 20,
			20895 + 20,
			20896 + 20,
			20897 + 20,
		},
		{
			20898 + 20,
			20899 + 20,
			20900 + 20,
			20901 + 20,
			20902 + 20,
		},
		{
			20903 + 20,
			20904 + 20,
			20905 + 20,
			20906 + 20,
			20907 + 20,
		}
	};
	track_paint_util_right_quarter_turn_5_tiles_paint(2, height, direction, trackSequence, RCT2_GLOBAL(0x00F44198, uint32), spriteIds, get_current_rotation());
	track_paint_util_right_quarter_turn_5_tiles_paint(0, height, direction, trackSequence, RCT2_GLOBAL(0x00F44198, uint32), sideSpriteIds, get_current_rotation());

	if (trackSequence != 1 && trackSequence != 4) {
		static const uint8 supportTypes[][7] = {
			{ 0, 0xFF, 4, 2, 0xFF, 4, 1 },
			{ 1, 0xFF, 5, 3, 0xFF, 5, 0 },
			{ 0, 0xFF, 2, 4, 0xFF, 2, 1 },
			{ 1, 0xFF, 3, 5, 0xFF, 3, 0 },
		};
		uint8 supportType = supportTypes[direction][trackSequence];
		wooden_a_supports_paint_setup(supportType, 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);
	}

	switch (trackSequence) {
	case 0: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
	case 1: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0); break;
	case 2: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 0xFFFF, 0); break;
	case 3: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0); break;
	case 4: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0); break;
	case 5: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 0xFFFF, 0); break;
	case 6: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
	}

	paint_util_set_general_support_height(height + 32, 0x20);
}

static void paint_river_rafts_track_s_bend(uint32 baseImageId, uint8 trackSequence, uint8 direction, int height, const uint8 supportTypes[][4], uint16 ssegs1, uint16 ssegs2)
{
	uint32 imageId = baseImageId;
	imageId += (direction & 1) * 4;
	imageId += (direction & 2) ? 35 : 0;
	imageId += (direction & 2) ? -trackSequence : trackSequence;
	imageId |= RCT2_GLOBAL(0x00F44198, uint32);

	uint32 sideImageId = imageId;
	sideImageId += (direction & 2) ? -16 : 16;

	switch (direction & 1) {
	case 0:
		switch (trackSequence) {
		case 0: sub_98197C(imageId    , 0, 0, 32, 27, 2, height, 0, 2, height     , get_current_rotation());
				sub_98197C(sideImageId, 0, 0, 32, 27, 0, height, 0, 2, height + 27, get_current_rotation()); break;
		case 1: sub_98197C(imageId    , 0, 0, 32, 26, 2, height, 0, 6, height     , get_current_rotation());
				sub_98197C(sideImageId, 0, 0, 32, 26, 0, height, 0, 6, height + 27, get_current_rotation()); break;
		case 2: sub_98197C(imageId    , 0, 0, 32, 26, 2, height, 0, 6, height     , get_current_rotation());
				sub_98197C(sideImageId, 0, 0, 32, 26, 0, height, 0, 6, height + 27, get_current_rotation()); break;
		case 3: sub_98197C(imageId    , 0, 0, 32, 27, 2, height, 0, 2, height     , get_current_rotation());
				sub_98197C(sideImageId, 0, 0, 32, 27, 0, height, 0, 2, height + 27, get_current_rotation()); break;
		}
	case 1:
		switch (trackSequence) {
		case 0: sub_98197C(imageId    , 0, 0, 27, 32, 2, height, 2, 0, height     , get_current_rotation());
				sub_98197C(sideImageId, 0, 0, 27, 32, 0, height, 2, 0, height + 27, get_current_rotation()); break;
		case 1: sub_98197C(imageId    , 0, 0, 26, 32, 2, height, 6, 0, height     , get_current_rotation());
				sub_98197C(sideImageId, 0, 0, 26, 32, 0, height, 6, 0, height + 27, get_current_rotation()); break;
		case 2: sub_98197C(imageId    , 0, 0, 26, 32, 2, height, 6, 0, height     , get_current_rotation());
				sub_98197C(sideImageId, 0, 0, 26, 32, 0, height, 6, 0, height + 27, get_current_rotation()); break;
		case 3: sub_98197C(imageId    , 0, 0, 27, 32, 2, height, 2, 0, height     , get_current_rotation());
				sub_98197C(sideImageId, 0, 0, 27, 32, 0, height, 2, 0, height + 27, get_current_rotation()); break;
		}
	}

	uint8 supportType = supportTypes[direction][trackSequence];
	wooden_a_supports_paint_setup(supportType, 0, height, RCT2_GLOBAL(0x00F4419C, uint32), NULL);

	switch (trackSequence) {
	case 0:
	case 3:
		paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
		break;
	case 1:
		paint_util_set_segment_support_height(paint_util_rotate_segments(ssegs1, direction), 0xFFFF, 0);
		break;
	case 2:
		paint_util_set_segment_support_height(paint_util_rotate_segments(ssegs2, direction), 0xFFFF, 0);
		break;
	}

	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0089B180 */
static void paint_river_rafts_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint8 supportTypes[][4] = {
		{ 0, 5, 3, 0 },
		{ 1, 2, 4, 1 },
		{ 0, 3, 5, 0 },
		{ 1, 4, 2, 1 },
	};
	paint_river_rafts_track_s_bend(20948, trackSequence, direction, height, supportTypes,
		SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4,
		SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC);
}

/** rct2: 0x0089B190 */
static void paint_river_rafts_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint8 supportTypes[][4] = {
		{ 0, 4, 2, 0 },
		{ 1, 5, 3, 1 },
		{ 0, 2, 4, 0 },
		{ 1, 3, 5, 1 },
	};
	paint_river_rafts_track_s_bend(20956, trackSequence, direction, height, supportTypes,
		SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC,
		SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4);
}

/**
 * rct2: 0x0089B0C0
 */
TRACK_PAINT_FUNCTION get_track_paint_function_river_rafts(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT: return paint_river_rafts_track_flat;

	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
		return paint_river_rafts_station;

	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:	return paint_river_rafts_track_left_quarter_turn_5_tiles;
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:	return paint_river_rafts_track_right_quarter_turn_5_tiles;

	case TRACK_ELEM_S_BEND_LEFT:	return paint_river_rafts_track_s_bend_left;
	case TRACK_ELEM_S_BEND_RIGHT:	return paint_river_rafts_track_s_bend_right;
	}
	return NULL;
}
