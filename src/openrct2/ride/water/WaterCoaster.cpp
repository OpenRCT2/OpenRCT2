/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../common.h"
#include "../coaster/JuniorRollerCoaster.h"
#include "../Track.h"
#include "../TrackPaint.h"

static void water_rc_track_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_flat(session, rideIndex, trackSequence, direction, height, tileElement,
                               isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                    isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_flat_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_flat_to_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                            isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_25_deg_up_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, tileElement,
                                            isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    water_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void water_rc_track_flat_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    water_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void water_rc_track_25_deg_down_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    water_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void water_rc_track_diag_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_diag_flat(session, rideIndex, trackSequence, direction, height, tileElement,
                                    isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_diag_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                         isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_flat_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_diag_flat_to_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                                 isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_up_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_diag_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, tileElement,
                                                 isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_diag_25_deg_down(session, rideIndex, trackSequence, direction, height, tileElement,
                                           isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_flat_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_diag_flat_to_25_deg_down(session, rideIndex, trackSequence, direction, height, tileElement,
                                                   isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_down_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    bool isChained = track_element_is_lift_hill(tileElement);
    junior_rc_paint_track_diag_25_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, tileElement,
                                                   isChained ? JUNIOR_RC_CHAIN_CHAIN_LIFT : JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_station(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_station(session, rideIndex, trackSequence, direction, height, tileElement, RIDE_TYPE_WATER_COASTER);
}

static void water_rc_track_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_60_deg_up(session, rideIndex, trackSequence, direction, height, tileElement, JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_25_deg_up_to_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                                 JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_60_deg_up_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                                 JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    water_rc_track_60_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, tileElement);
}

static void water_rc_track_25_deg_down_to_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    water_rc_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, tileElement);
}

static void water_rc_track_60_deg_down_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    water_rc_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, tileElement);
}

// 5 tile turns

static void water_rc_track_left_quarter_turn_5_tiles_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_left_quarter_turn_5_tiles_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                                              JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_right_quarter_turn_5_tiles_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_right_quarter_turn_5_tiles_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                                               JUNIOR_RC_CHAIN_NONE);
}

static constexpr const uint8_t water_rc_left_quarter_turn_5_tiles_to_right_turn_map[] = { 6, 4, 5, 3, 1, 2, 0 };

static void water_rc_track_left_quarter_turn_5_tiles_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    water_rc_track_right_quarter_turn_5_tiles_25_deg_up(session, rideIndex,
                                                        water_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence],
                                                        (direction + 1) % 4, height, tileElement);
}

static void water_rc_track_right_quarter_turn_5_tiles_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    water_rc_track_left_quarter_turn_5_tiles_25_deg_up(session, rideIndex,
                                                       water_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence],
                                                       (direction + 3) % 4, height, tileElement);
}

// 3 tile turns

static void water_rc_track_right_quarter_turn_3_tiles_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_right_quarter_turn_3_tiles_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                                               JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_right_quarter_turn_3_tiles_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_right_quarter_turn_3_tiles_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                 tileElement, JUNIOR_RC_CHAIN_NONE);
}

static constexpr const uint8_t water_rc_left_quarter_turn_3_tiles_to_right_turn_map[] = { 3, 1, 2, 0 };

static void water_rc_track_left_quarter_turn_3_tiles_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    trackSequence = water_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    water_rc_track_right_quarter_turn_3_tiles_25_deg_down(session, rideIndex, trackSequence, (direction + 1) % 4, height,
                                                          tileElement);
}

static void water_rc_track_left_quarter_turn_3_tiles_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    trackSequence = water_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    water_rc_track_right_quarter_turn_3_tiles_25_deg_up(session, rideIndex, trackSequence, (direction + 1) % 4, height,
                                                        tileElement);
}

static void water_rc_track_diag_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_diag_60_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                         JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_up_to_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_diag_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                                      JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_60_deg_up_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_diag_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement,
                                                      JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_diag_60_deg_down(session, rideIndex, trackSequence, direction, height, tileElement,
                                           JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_25_deg_down_to_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_diag_25_deg_down_to_60_deg_down(session, rideIndex, trackSequence, direction, height, tileElement,
                                                          JUNIOR_RC_CHAIN_NONE);
}

static void water_rc_track_diag_60_deg_down_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    junior_rc_paint_track_diag_60_deg_down_to_25_deg_down(session, rideIndex, trackSequence, direction, height, tileElement,
                                                          JUNIOR_RC_CHAIN_NONE);
}

TRACK_PAINT_FUNCTION get_track_paint_function_water_rc(int32_t trackType, int32_t direction)
{
    switch (trackType)
    {
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
    return nullptr;
}
