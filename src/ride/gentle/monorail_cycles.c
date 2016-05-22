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
#include "../track_paint.h"
#include "../track.h"
#include "../vehicle_paint.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../../world/map.h"

/** rct2: 0x*/
static void paint_monorail_cycles_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_left_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_right_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_left_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_right_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
}

/**
 * rct2: 0x0088ac88
 */
TRACK_PAINT_FUNCTION get_track_paint_function_monorail_cycles(int trackType, int direction) {
	switch(trackType) {
		case TRACK_ELEM_FLAT:
			return paint_monorail_cycles_track_flat;

		case TRACK_ELEM_END_STATION:
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
			return paint_monorail_cycles_station;

		case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
			return paint_monorail_cycles_track_left_quarter_turn_5_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
			return paint_monorail_cycles_track_right_quarter_turn_5_tiles;

		case TRACK_ELEM_S_BEND_LEFT:
			return paint_monorail_cycles_track_s_bend_left;
		case TRACK_ELEM_S_BEND_RIGHT:
			return paint_monorail_cycles_track_s_bend_right;

		case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
			return paint_monorail_cycles_track_left_quarter_turn_3_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
			return paint_monorail_cycles_track_right_quarter_turn_3_tiles;
	}

	return NULL;
}
