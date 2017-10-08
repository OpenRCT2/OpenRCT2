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

#include "../../common.h"
#include "../coaster/junior_roller_coaster.h"
#include "../track.h"
#include "../track_paint.h"

static void water_rc_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_flat(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_flat_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    water_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void water_rc_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    water_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void water_rc_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    water_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void water_rc_track_diag_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_diag_flat(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_diag_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_diag_flat_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_diag_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_diag_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_diag_flat_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isChained = track_element_is_lift_hill(mapElement);
    junior_rc_paint_track_diag_25_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_station(session, rideIndex, trackSequence, direction, height, mapElement, RIDE_TYPE_WATER_COASTER);
}

static void water_rc_track_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    water_rc_track_60_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

static void water_rc_track_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    water_rc_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

static void water_rc_track_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    water_rc_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

// 5 tile turns

static void water_rc_track_left_quarter_turn_5_tiles_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_left_quarter_turn_5_tiles_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_right_quarter_turn_5_tiles_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_right_quarter_turn_5_tiles_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static const uint8 water_rc_left_quarter_turn_5_tiles_to_right_turn_map[] = {6, 4, 5, 3, 1, 2, 0};

static void water_rc_track_left_quarter_turn_5_tiles_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    water_rc_track_right_quarter_turn_5_tiles_25_deg_up(session, rideIndex, water_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence], (direction + 1) % 4, height, mapElement);}

static void water_rc_track_right_quarter_turn_5_tiles_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    water_rc_track_left_quarter_turn_5_tiles_25_deg_up(session, rideIndex, water_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence], (direction + 3) % 4, height, mapElement);
}

// 3 tile turns

static void water_rc_track_right_quarter_turn_3_tiles_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_right_quarter_turn_3_tiles_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_right_quarter_turn_3_tiles_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_right_quarter_turn_3_tiles_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static const uint8 water_rc_left_quarter_turn_3_tiles_to_right_turn_map[] = {3, 1, 2, 0};

static void water_rc_track_left_quarter_turn_3_tiles_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = water_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    water_rc_track_right_quarter_turn_3_tiles_25_deg_down(session, rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

static void water_rc_track_left_quarter_turn_3_tiles_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = water_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    water_rc_track_right_quarter_turn_3_tiles_25_deg_up(session, rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}


static void water_rc_track_diag_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_diag_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_diag_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_diag_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_diag_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_diag_25_deg_down_to_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    junior_rc_paint_track_diag_60_deg_down_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, JUNIOR_RC_CHAIN_NONE);
}

TRACK_PAINT_FUNCTION get_track_paint_function_water_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return water_rc_track_flat;
    case TRACK_ELEM_25_DEG_UP:
        return water_rc_track_25_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return water_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return water_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return water_rc_track_25_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return water_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return water_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_DIAG_FLAT:
        return water_rc_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return water_rc_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return water_rc_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return water_rc_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return water_rc_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return water_rc_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return water_rc_track_diag_25_deg_down_to_flat;

    // Use Junior RC without lift hill
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return water_rc_track_station;
    case TRACK_ELEM_60_DEG_UP:
        return water_rc_track_60_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return water_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return water_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_60_DEG_DOWN:
        return water_rc_track_60_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return water_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return water_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return water_rc_track_left_quarter_turn_5_tiles_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return water_rc_track_right_quarter_turn_5_tiles_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return water_rc_track_left_quarter_turn_5_tiles_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return water_rc_track_right_quarter_turn_5_tiles_25_deg_down;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return water_rc_track_left_quarter_turn_3_tiles_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return water_rc_track_right_quarter_turn_3_tiles_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return water_rc_track_left_quarter_turn_3_tiles_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return water_rc_track_right_quarter_turn_3_tiles_25_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_UP:
        return water_rc_track_diag_60_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
        return water_rc_track_diag_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
        return water_rc_track_diag_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_DOWN:
        return water_rc_track_diag_60_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
        return water_rc_track_diag_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
        return water_rc_track_diag_60_deg_down_to_25_deg_down;

    //////////////////////////////////////////////////////
    // Use Junior RC
    //////////////////////////////////////////////////////
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
    case TRACK_ELEM_S_BEND_LEFT:
    case TRACK_ELEM_S_BEND_RIGHT:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
    case TRACK_ELEM_FLAT_TO_60_DEG_UP:
    case TRACK_ELEM_60_DEG_UP_TO_FLAT:
    case TRACK_ELEM_FLAT_TO_60_DEG_DOWN:
    case TRACK_ELEM_60_DEG_DOWN_TO_FLAT:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
    case TRACK_ELEM_BRAKES:
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
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
        return get_track_paint_function_junior_rc(trackType, direction);

    //////////////////////////////////////////////////////
    // Use River Rafts
    //////////////////////////////////////////////////////
    case TRACK_ELEM_FLAT_COVERED:
        return get_track_paint_function_splash_boats(TRACK_ELEM_FLAT, direction);
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED:
        return get_track_paint_function_splash_boats(TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES, direction);
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED:
        return get_track_paint_function_splash_boats(TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES, direction);
    }
    return NULL;
}
