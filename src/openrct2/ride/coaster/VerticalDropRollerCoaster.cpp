/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../paint/Supports.h"
#include "../TrackData.h"
#include "BolligerMabillardTrack.h"

/** rct2: 0x008AA00C */
static void vertical_drop_rc_track_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_station(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_station(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA01C */
static void vertical_drop_rc_track_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA02C */
static void vertical_drop_rc_track_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA03C */
static void vertical_drop_rc_track_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA04C */
static void vertical_drop_rc_track_25_deg_up_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA05C */
static void vertical_drop_rc_track_60_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA06C */
static void vertical_drop_rc_track_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA07C */
static void vertical_drop_rc_track_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA08C */
static void vertical_drop_rc_track_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA09C */
static void vertical_drop_rc_track_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0AC */
static void vertical_drop_rc_track_25_deg_down_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0BC */
static void vertical_drop_rc_track_60_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_down_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0CC */
static void vertical_drop_rc_track_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0DC */
static void vertical_drop_rc_track_left_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_5(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0EC */
static void vertical_drop_rc_track_right_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_5(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0FC */
static void vertical_drop_rc_track_flat_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA10C */
static void vertical_drop_rc_track_flat_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA11C */
static void vertical_drop_rc_track_left_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_bank_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA12C */
static void vertical_drop_rc_track_right_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_bank_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA13C */
static void vertical_drop_rc_track_banked_left_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_banked_left_quarter_turn_5(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA14C */
static void vertical_drop_rc_track_banked_right_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_banked_right_quarter_turn_5(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA15C */
static void vertical_drop_rc_track_left_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_bank_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA16C */
static void vertical_drop_rc_track_right_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_bank_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA17C */
static void vertical_drop_rc_track_25_deg_up_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA18C */
static void vertical_drop_rc_track_25_deg_up_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA19C */
static void vertical_drop_rc_track_left_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_bank_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1AC */
static void vertical_drop_rc_track_right_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_bank_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1BC */
static void vertical_drop_rc_track_25_deg_down_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1CC */
static void vertical_drop_rc_track_25_deg_down_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1DC */
static void vertical_drop_rc_track_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_bank(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1EC */
static void vertical_drop_rc_track_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1FC */
static void vertical_drop_rc_track_left_quarter_turn_5_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA20C */
static void vertical_drop_rc_track_right_quarter_turn_5_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA21C */
static void vertical_drop_rc_track_left_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_5_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA22C */
static void vertical_drop_rc_track_right_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_5_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA23C */
static void vertical_drop_rc_track_s_bend_left(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_s_bend_left(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA24C */
static void vertical_drop_rc_track_s_bend_right(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_s_bend_right(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA49C */
static void vertical_drop_rc_track_left_vertical_loop(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_vertical_loop(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4AC */
static void vertical_drop_rc_track_right_vertical_loop(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_vertical_loop(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA28C */
static void vertical_drop_rc_track_left_quarter_turn_3(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_3(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA29C */
static void vertical_drop_rc_track_right_quarter_turn_3(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_3(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2AC */
static void vertical_drop_rc_track_left_quarter_turn_3_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2BC */
static void vertical_drop_rc_track_right_quarter_turn_3_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2CC */
static void vertical_drop_rc_track_left_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2DC */
static void vertical_drop_rc_track_right_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2EC */
static void vertical_drop_rc_track_left_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2FC */
static void vertical_drop_rc_track_right_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA30C */
static void vertical_drop_rc_track_left_half_banked_helix_up_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_half_banked_helix_up_small(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA31C */
static void vertical_drop_rc_track_right_half_banked_helix_up_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_half_banked_helix_up_small(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA32C */
static void vertical_drop_rc_track_left_half_banked_helix_down_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_half_banked_helix_down_small(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA33C */
static void vertical_drop_rc_track_right_half_banked_helix_down_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_half_banked_helix_down_small(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA34C */
static void vertical_drop_rc_track_left_half_banked_helix_up_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_half_banked_helix_up_large(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA35C */
static void vertical_drop_rc_track_right_half_banked_helix_up_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_half_banked_helix_up_large(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA36C */
static void vertical_drop_rc_track_left_half_banked_helix_down_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_half_banked_helix_down_large(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA37C */
static void vertical_drop_rc_track_right_half_banked_helix_down_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_half_banked_helix_down_large(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA3AC */
static void vertical_drop_rc_track_left_quarter_turn_1_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA38C */
static void vertical_drop_rc_track_right_quarter_turn_1_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA39C */
static void vertical_drop_rc_track_left_quarter_turn_1_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA3BC */
static void vertical_drop_rc_track_right_quarter_turn_1_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA40C */
static void vertical_drop_rc_track_brakes(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_brakes(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7EC */
static void vertical_drop_rc_track_25_deg_up_left_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_left_banked(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7FC */
static void vertical_drop_rc_track_25_deg_up_right_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_right_banked(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA41C */
static void vertical_drop_rc_track_on_ride_photo(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_on_ride_photo(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA80C */
static void vertical_drop_rc_track_25_deg_down_left_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_left_banked(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA81C */
static void vertical_drop_rc_track_25_deg_down_right_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_right_banked(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA42C */
static void vertical_drop_rc_track_90_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_90_deg_up(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA43C */
static void vertical_drop_rc_track_90_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_90_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA44C */
static void vertical_drop_rc_track_60_deg_up_to_90_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up_to_90_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA45C */
static void vertical_drop_rc_track_90_deg_down_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_90_deg_down_to_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA46C */
static void vertical_drop_rc_track_90_deg_up_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_90_deg_up_to_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA47C */
static void vertical_drop_rc_track_60_deg_down_to_90_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_down_to_90_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4CC */
static void vertical_drop_rc_track_left_eighth_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_eighth_to_diag(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4DC */
static void vertical_drop_rc_track_right_eighth_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_eighth_to_diag(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4EC */
static void vertical_drop_rc_track_left_eighth_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_eighth_to_orthogonal(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4FC */
static void vertical_drop_rc_track_right_eighth_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_eighth_to_orthogonal(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA50C */
static void vertical_drop_rc_track_left_eighth_bank_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_eighth_bank_to_diag(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA51C */
static void vertical_drop_rc_track_right_eighth_bank_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_eighth_bank_to_diag(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA52C */
static void vertical_drop_rc_track_left_eighth_bank_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_eighth_bank_to_orthogonal(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA53C */
static void vertical_drop_rc_track_right_eighth_bank_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_eighth_bank_to_orthogonal(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4BC */
static void vertical_drop_rc_track_diag_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA56C */
static void vertical_drop_rc_track_diag_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5CC */
static void vertical_drop_rc_track_diag_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA54C */
static void vertical_drop_rc_track_diag_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5AC */
static void vertical_drop_rc_track_diag_25_deg_up_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5BC */
static void vertical_drop_rc_track_diag_60_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA55C */
static void vertical_drop_rc_track_diag_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA59C */
static void vertical_drop_rc_track_diag_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5FC */
static void vertical_drop_rc_track_diag_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA57C */
static void vertical_drop_rc_track_diag_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5DC */
static void vertical_drop_rc_track_diag_25_deg_down_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5EC */
static void vertical_drop_rc_track_diag_60_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_down_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA58C */
static void vertical_drop_rc_track_diag_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA62C */
static void vertical_drop_rc_track_diag_flat_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA63C */
static void vertical_drop_rc_track_diag_flat_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA64C */
static void vertical_drop_rc_track_diag_left_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_left_bank_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA65C */
static void vertical_drop_rc_track_diag_right_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_right_bank_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA68C */
static void vertical_drop_rc_track_diag_left_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_left_bank_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA69C */
static void vertical_drop_rc_track_diag_right_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_right_bank_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA66C */
static void vertical_drop_rc_track_diag_25_deg_up_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA67C */
static void vertical_drop_rc_track_diag_25_deg_up_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6AC */
static void vertical_drop_rc_track_diag_left_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_left_bank_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6BC */
static void vertical_drop_rc_track_diag_right_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_right_bank_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6CC */
static void vertical_drop_rc_track_diag_25_deg_down_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6DC */
static void vertical_drop_rc_track_diag_25_deg_down_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA60C */
static void vertical_drop_rc_track_diag_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA61C */
static void vertical_drop_rc_track_diag_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA72C */
static void vertical_drop_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_bank_to_left_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA73C */
static void vertical_drop_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_bank_to_right_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA74C */
static void vertical_drop_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_down_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA75C */
static void vertical_drop_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_down_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA9EC */
static void vertical_drop_rc_track_block_brakes(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_block_brakes(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA96C */
static void vertical_drop_rc_track_left_banked_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA97C */
static void vertical_drop_rc_track_right_banked_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA98C */
static void vertical_drop_rc_track_left_banked_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA99C */
static void vertical_drop_rc_track_right_banked_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8AC */
static void vertical_drop_rc_track_left_banked_quarter_turn_5_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8BC */
static void vertical_drop_rc_track_right_banked_quarter_turn_5_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8CC */
static void vertical_drop_rc_track_left_banked_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8DC */
static void vertical_drop_rc_track_right_banked_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_5_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA82C */
static void vertical_drop_rc_track_25_deg_up_to_left_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_left_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA83C */
static void vertical_drop_rc_track_25_deg_up_to_right_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_right_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA84C */
static void vertical_drop_rc_track_left_banked_25_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA85C */
static void vertical_drop_rc_track_right_banked_25_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA86C */
static void vertical_drop_rc_track_25_deg_down_to_left_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_left_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA87C */
static void vertical_drop_rc_track_25_deg_down_to_right_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_right_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA88C */
static void vertical_drop_rc_track_left_banked_25_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA89C */
static void vertical_drop_rc_track_right_banked_25_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8EC */
static void vertical_drop_rc_track_left_banked_flat_to_left_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_flat_to_left_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8FC */
static void vertical_drop_rc_track_right_banked_flat_to_right_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_flat_to_right_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA92C */
static void vertical_drop_rc_track_left_banked_25_deg_up_to_left_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_left_banked_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA93C */
static void vertical_drop_rc_track_right_banked_25_deg_up_to_right_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_right_banked_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA94C */
static void vertical_drop_rc_track_left_banked_flat_to_left_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_flat_to_left_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA95C */
static void vertical_drop_rc_track_right_banked_flat_to_right_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_flat_to_right_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA90C */
static void vertical_drop_rc_track_left_banked_25_deg_down_to_left_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_left_banked_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA91C */
static void vertical_drop_rc_track_right_banked_25_deg_down_to_right_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_right_banked_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA76C */
static void vertical_drop_rc_track_flat_to_left_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_left_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA77C */
static void vertical_drop_rc_track_flat_to_right_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_right_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA78C */
static void vertical_drop_rc_track_left_banked_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA79C */
static void vertical_drop_rc_track_right_banked_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7AC */
static void vertical_drop_rc_track_flat_to_left_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_left_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7BC */
static void vertical_drop_rc_track_flat_to_right_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_right_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7CC */
static void vertical_drop_rc_track_left_banked_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7DC */
static void vertical_drop_rc_track_right_banked_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA9AC */
static void vertical_drop_rc_track_left_quarter_turn_1_90_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_90_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA9BC */
static void vertical_drop_rc_track_right_quarter_turn_1_90_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_90_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA9CC */
static void vertical_drop_rc_track_left_quarter_turn_1_90_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_90_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA9DC */
static void vertical_drop_rc_track_right_quarter_turn_1_90_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_90_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/* The following elements used to be specific to the vertical_drop RC */
/** rct2: 0x008AA3CC */
static void vertical_drop_rc_track_flat_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA3DC */
static void vertical_drop_rc_track_60_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA3EC */
static void vertical_drop_rc_track_flat_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA3FC */
static void vertical_drop_rc_track_60_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA48C */
static void vertical_drop_rc_track_brake_for_drop(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_brake_for_drop(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6EC */
static void vertical_drop_rc_track_diag_flat_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6FC */
static void vertical_drop_rc_track_diag_60_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA70C */
static void vertical_drop_rc_track_diag_flat_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA71C */
static void vertical_drop_rc_track_diag_60_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

/* The following elements used to be specific to the steel twister RC */
static void vertical_drop_rc_track_half_loop_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_half_loop_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_half_loop_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_half_loop_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_corkscrew_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_corkscrew_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_corkscrew_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_corkscrew_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_corkscrew_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_corkscrew_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_corkscrew_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_corkscrew_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_flat_to_60_deg_up_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up_long_base(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_60_deg_up_to_flat_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat_long_base(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_60_deg_down_to_flat_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_60_deg_down_long_base(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_flat_to_60_deg_down_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat_long_base122(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_barrel_roll_up_to_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_barrel_roll_up_to_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_barrel_roll_up_to_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_barrel_roll_up_to_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_barrel_roll_down_to_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_barrel_roll_down_to_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_barrel_roll_down_to_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_barrel_roll_down_to_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_powered_lift(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_powered_lift(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_large_half_loop_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_large_half_loop_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_large_half_loop_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_large_half_loop_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_large_half_loop_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_large_half_loop_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_large_half_loop_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_large_half_loop_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_90_deg_to_inverted_flat_quarter_loop_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_90_deg_to_inverted_flat_quarter_loop_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_inverted_flat_to_90_deg_quarter_loop_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_inverted_flat_to_90_deg_quarter_loop_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_booster(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_booster(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_BOXED);
}

TRACK_PAINT_FUNCTION get_track_paint_function_vertical_drop_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return vertical_drop_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return vertical_drop_rc_track_station;
        case TrackElemType::Up25:
            return vertical_drop_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return vertical_drop_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return vertical_drop_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return vertical_drop_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return vertical_drop_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return vertical_drop_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return vertical_drop_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return vertical_drop_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return vertical_drop_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return vertical_drop_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return vertical_drop_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return vertical_drop_rc_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return vertical_drop_rc_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return vertical_drop_rc_track_right_quarter_turn_5;
        case TrackElemType::FlatToLeftBank:
            return vertical_drop_rc_track_flat_to_left_bank;
        case TrackElemType::FlatToRightBank:
            return vertical_drop_rc_track_flat_to_right_bank;
        case TrackElemType::LeftBankToFlat:
            return vertical_drop_rc_track_left_bank_to_flat;
        case TrackElemType::RightBankToFlat:
            return vertical_drop_rc_track_right_bank_to_flat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return vertical_drop_rc_track_banked_left_quarter_turn_5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return vertical_drop_rc_track_banked_right_quarter_turn_5;
        case TrackElemType::LeftBankToUp25:
            return vertical_drop_rc_track_left_bank_to_25_deg_up;
        case TrackElemType::RightBankToUp25:
            return vertical_drop_rc_track_right_bank_to_25_deg_up;
        case TrackElemType::Up25ToLeftBank:
            return vertical_drop_rc_track_25_deg_up_to_left_bank;
        case TrackElemType::Up25ToRightBank:
            return vertical_drop_rc_track_25_deg_up_to_right_bank;
        case TrackElemType::LeftBankToDown25:
            return vertical_drop_rc_track_left_bank_to_25_deg_down;
        case TrackElemType::RightBankToDown25:
            return vertical_drop_rc_track_right_bank_to_25_deg_down;
        case TrackElemType::Down25ToLeftBank:
            return vertical_drop_rc_track_25_deg_down_to_left_bank;
        case TrackElemType::Down25ToRightBank:
            return vertical_drop_rc_track_25_deg_down_to_right_bank;
        case TrackElemType::LeftBank:
            return vertical_drop_rc_track_left_bank;
        case TrackElemType::RightBank:
            return vertical_drop_rc_track_right_bank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return vertical_drop_rc_track_left_quarter_turn_5_25_deg_up;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return vertical_drop_rc_track_right_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return vertical_drop_rc_track_left_quarter_turn_5_25_deg_down;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return vertical_drop_rc_track_right_quarter_turn_5_25_deg_down;
        case TrackElemType::SBendLeft:
            return vertical_drop_rc_track_s_bend_left;
        case TrackElemType::SBendRight:
            return vertical_drop_rc_track_s_bend_right;
        case TrackElemType::LeftVerticalLoop:
            return vertical_drop_rc_track_left_vertical_loop;
        case TrackElemType::RightVerticalLoop:
            return vertical_drop_rc_track_right_vertical_loop;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return vertical_drop_rc_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return vertical_drop_rc_track_right_quarter_turn_3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return vertical_drop_rc_track_left_quarter_turn_3_bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return vertical_drop_rc_track_right_quarter_turn_3_bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return vertical_drop_rc_track_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return vertical_drop_rc_track_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return vertical_drop_rc_track_left_quarter_turn_3_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return vertical_drop_rc_track_right_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return vertical_drop_rc_track_left_half_banked_helix_up_small;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return vertical_drop_rc_track_right_half_banked_helix_up_small;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return vertical_drop_rc_track_left_half_banked_helix_down_small;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return vertical_drop_rc_track_right_half_banked_helix_down_small;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return vertical_drop_rc_track_left_half_banked_helix_up_large;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return vertical_drop_rc_track_right_half_banked_helix_up_large;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return vertical_drop_rc_track_left_half_banked_helix_down_large;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return vertical_drop_rc_track_right_half_banked_helix_down_large;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return vertical_drop_rc_track_left_quarter_turn_1_60_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return vertical_drop_rc_track_right_quarter_turn_1_60_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return vertical_drop_rc_track_left_quarter_turn_1_60_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return vertical_drop_rc_track_right_quarter_turn_1_60_deg_down;
        case TrackElemType::Brakes:
            return vertical_drop_rc_track_brakes;
        case TrackElemType::Up25LeftBanked:
            return vertical_drop_rc_track_25_deg_up_left_banked;
        case TrackElemType::Up25RightBanked:
            return vertical_drop_rc_track_25_deg_up_right_banked;
        case TrackElemType::OnRidePhoto:
            return vertical_drop_rc_track_on_ride_photo;
        case TrackElemType::Down25LeftBanked:
            return vertical_drop_rc_track_25_deg_down_left_banked;
        case TrackElemType::Down25RightBanked:
            return vertical_drop_rc_track_25_deg_down_right_banked;
        case TrackElemType::Up90:
            return vertical_drop_rc_track_90_deg_up;
        case TrackElemType::Down90:
            return vertical_drop_rc_track_90_deg_down;
        case TrackElemType::Up60ToUp90:
            return vertical_drop_rc_track_60_deg_up_to_90_deg_up;
        case TrackElemType::Down90ToDown60:
            return vertical_drop_rc_track_90_deg_down_to_60_deg_down;
        case TrackElemType::Up90ToUp60:
            return vertical_drop_rc_track_90_deg_up_to_60_deg_up;
        case TrackElemType::Down60ToDown90:
            return vertical_drop_rc_track_60_deg_down_to_90_deg_down;
        case TrackElemType::LeftEighthToDiag:
            return vertical_drop_rc_track_left_eighth_to_diag;
        case TrackElemType::RightEighthToDiag:
            return vertical_drop_rc_track_right_eighth_to_diag;
        case TrackElemType::LeftEighthToOrthogonal:
            return vertical_drop_rc_track_left_eighth_to_orthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return vertical_drop_rc_track_right_eighth_to_orthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return vertical_drop_rc_track_left_eighth_bank_to_diag;
        case TrackElemType::RightEighthBankToDiag:
            return vertical_drop_rc_track_right_eighth_bank_to_diag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return vertical_drop_rc_track_left_eighth_bank_to_orthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return vertical_drop_rc_track_right_eighth_bank_to_orthogonal;
        case TrackElemType::DiagFlat:
            return vertical_drop_rc_track_diag_flat;
        case TrackElemType::DiagUp25:
            return vertical_drop_rc_track_diag_25_deg_up;
        case TrackElemType::DiagUp60:
            return vertical_drop_rc_track_diag_60_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return vertical_drop_rc_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToUp60:
            return vertical_drop_rc_track_diag_25_deg_up_to_60_deg_up;
        case TrackElemType::DiagUp60ToUp25:
            return vertical_drop_rc_track_diag_60_deg_up_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return vertical_drop_rc_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return vertical_drop_rc_track_diag_25_deg_down;
        case TrackElemType::DiagDown60:
            return vertical_drop_rc_track_diag_60_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return vertical_drop_rc_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToDown60:
            return vertical_drop_rc_track_diag_25_deg_down_to_60_deg_down;
        case TrackElemType::DiagDown60ToDown25:
            return vertical_drop_rc_track_diag_60_deg_down_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return vertical_drop_rc_track_diag_25_deg_down_to_flat;
        case TrackElemType::DiagFlatToLeftBank:
            return vertical_drop_rc_track_diag_flat_to_left_bank;
        case TrackElemType::DiagFlatToRightBank:
            return vertical_drop_rc_track_diag_flat_to_right_bank;
        case TrackElemType::DiagLeftBankToFlat:
            return vertical_drop_rc_track_diag_left_bank_to_flat;
        case TrackElemType::DiagRightBankToFlat:
            return vertical_drop_rc_track_diag_right_bank_to_flat;
        case TrackElemType::DiagLeftBankToUp25:
            return vertical_drop_rc_track_diag_left_bank_to_25_deg_up;
        case TrackElemType::DiagRightBankToUp25:
            return vertical_drop_rc_track_diag_right_bank_to_25_deg_up;
        case TrackElemType::DiagUp25ToLeftBank:
            return vertical_drop_rc_track_diag_25_deg_up_to_left_bank;
        case TrackElemType::DiagUp25ToRightBank:
            return vertical_drop_rc_track_diag_25_deg_up_to_right_bank;
        case TrackElemType::DiagLeftBankToDown25:
            return vertical_drop_rc_track_diag_left_bank_to_25_deg_down;
        case TrackElemType::DiagRightBankToDown25:
            return vertical_drop_rc_track_diag_right_bank_to_25_deg_down;
        case TrackElemType::DiagDown25ToLeftBank:
            return vertical_drop_rc_track_diag_25_deg_down_to_left_bank;
        case TrackElemType::DiagDown25ToRightBank:
            return vertical_drop_rc_track_diag_25_deg_down_to_right_bank;
        case TrackElemType::DiagLeftBank:
            return vertical_drop_rc_track_diag_left_bank;
        case TrackElemType::DiagRightBank:
            return vertical_drop_rc_track_diag_right_bank;
        case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
            return vertical_drop_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
            return vertical_drop_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
            return vertical_drop_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank;
        case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
            return vertical_drop_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank;
        case TrackElemType::BlockBrakes:
            return vertical_drop_rc_track_block_brakes;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
            return vertical_drop_rc_track_left_banked_quarter_turn_3_25_deg_up;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
            return vertical_drop_rc_track_right_banked_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return vertical_drop_rc_track_left_banked_quarter_turn_3_25_deg_down;
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return vertical_drop_rc_track_right_banked_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
            return vertical_drop_rc_track_left_banked_quarter_turn_5_25_deg_up;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
            return vertical_drop_rc_track_right_banked_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return vertical_drop_rc_track_left_banked_quarter_turn_5_25_deg_down;
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return vertical_drop_rc_track_right_banked_quarter_turn_5_25_deg_down;
        case TrackElemType::Up25ToLeftBankedUp25:
            return vertical_drop_rc_track_25_deg_up_to_left_banked_25_deg_up;
        case TrackElemType::Up25ToRightBankedUp25:
            return vertical_drop_rc_track_25_deg_up_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToUp25:
            return vertical_drop_rc_track_left_banked_25_deg_up_to_25_deg_up;
        case TrackElemType::RightBankedUp25ToUp25:
            return vertical_drop_rc_track_right_banked_25_deg_up_to_25_deg_up;
        case TrackElemType::Down25ToLeftBankedDown25:
            return vertical_drop_rc_track_25_deg_down_to_left_banked_25_deg_down;
        case TrackElemType::Down25ToRightBankedDown25:
            return vertical_drop_rc_track_25_deg_down_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToDown25:
            return vertical_drop_rc_track_left_banked_25_deg_down_to_25_deg_down;
        case TrackElemType::RightBankedDown25ToDown25:
            return vertical_drop_rc_track_right_banked_25_deg_down_to_25_deg_down;
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
            return vertical_drop_rc_track_left_banked_flat_to_left_banked_25_deg_up;
        case TrackElemType::RightBankedFlatToRightBankedUp25:
            return vertical_drop_rc_track_right_banked_flat_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
            return vertical_drop_rc_track_left_banked_25_deg_up_to_left_banked_flat;
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
            return vertical_drop_rc_track_right_banked_25_deg_up_to_right_banked_flat;
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
            return vertical_drop_rc_track_left_banked_flat_to_left_banked_25_deg_down;
        case TrackElemType::RightBankedFlatToRightBankedDown25:
            return vertical_drop_rc_track_right_banked_flat_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
            return vertical_drop_rc_track_left_banked_25_deg_down_to_left_banked_flat;
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
            return vertical_drop_rc_track_right_banked_25_deg_down_to_right_banked_flat;
        case TrackElemType::FlatToLeftBankedUp25:
            return vertical_drop_rc_track_flat_to_left_banked_25_deg_up;
        case TrackElemType::FlatToRightBankedUp25:
            return vertical_drop_rc_track_flat_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToFlat:
            return vertical_drop_rc_track_left_banked_25_deg_up_to_flat;
        case TrackElemType::RightBankedUp25ToFlat:
            return vertical_drop_rc_track_right_banked_25_deg_up_to_flat;
        case TrackElemType::FlatToLeftBankedDown25:
            return vertical_drop_rc_track_flat_to_left_banked_25_deg_down;
        case TrackElemType::FlatToRightBankedDown25:
            return vertical_drop_rc_track_flat_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToFlat:
            return vertical_drop_rc_track_left_banked_25_deg_down_to_flat;
        case TrackElemType::RightBankedDown25ToFlat:
            return vertical_drop_rc_track_right_banked_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn1TileUp90:
            return vertical_drop_rc_track_left_quarter_turn_1_90_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp90:
            return vertical_drop_rc_track_right_quarter_turn_1_90_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown90:
            return vertical_drop_rc_track_left_quarter_turn_1_90_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown90:
            return vertical_drop_rc_track_right_quarter_turn_1_90_deg_down;
        /* These track elements used to be specific to the vertical RC */
        case TrackElemType::FlatToUp60:
            return vertical_drop_rc_track_flat_to_60_deg_up;
        case TrackElemType::Up60ToFlat:
            return vertical_drop_rc_track_60_deg_up_to_flat;
        case TrackElemType::FlatToDown60:
            return vertical_drop_rc_track_flat_to_60_deg_down;
        case TrackElemType::Down60ToFlat:
            return vertical_drop_rc_track_60_deg_down_to_flat;
        case TrackElemType::BrakeForDrop:
            return vertical_drop_rc_track_brake_for_drop;
        case TrackElemType::DiagFlatToUp60:
            return vertical_drop_rc_track_diag_flat_to_60_deg_up;
        case TrackElemType::DiagUp60ToFlat:
            return vertical_drop_rc_track_diag_60_deg_up_to_flat;
        case TrackElemType::DiagFlatToDown60:
            return vertical_drop_rc_track_diag_flat_to_60_deg_down;
        case TrackElemType::DiagDown60ToFlat:
            return vertical_drop_rc_track_diag_60_deg_down_to_flat;
        /* These track elements used to be specific to the steel vertical_drop */
        case TrackElemType::HalfLoopUp:
            return vertical_drop_rc_track_half_loop_up;
        case TrackElemType::HalfLoopDown:
            return vertical_drop_rc_track_half_loop_down;
        case TrackElemType::LeftCorkscrewUp:
            return vertical_drop_rc_track_left_corkscrew_up;
        case TrackElemType::RightCorkscrewUp:
            return vertical_drop_rc_track_right_corkscrew_up;
        case TrackElemType::LeftCorkscrewDown:
            return vertical_drop_rc_track_left_corkscrew_down;
        case TrackElemType::RightCorkscrewDown:
            return vertical_drop_rc_track_right_corkscrew_down;
        case TrackElemType::FlatToUp60LongBase:
            return vertical_drop_rc_track_flat_to_60_deg_up_long_base;
        case TrackElemType::Up60ToFlatLongBase:
            return vertical_drop_rc_track_60_deg_up_to_flat_long_base;
        case TrackElemType::Down60ToFlatLongBase:
            return vertical_drop_rc_track_60_deg_down_to_flat_long_base;
        case TrackElemType::FlatToDown60LongBase:
            return vertical_drop_rc_track_flat_to_60_deg_down_long_base;
        case TrackElemType::LeftBarrelRollUpToDown:
            return vertical_drop_rc_track_left_barrel_roll_up_to_down;
        case TrackElemType::RightBarrelRollUpToDown:
            return vertical_drop_rc_track_right_barrel_roll_up_to_down;
        case TrackElemType::LeftBarrelRollDownToUp:
            return vertical_drop_rc_track_left_barrel_roll_down_to_up;
        case TrackElemType::RightBarrelRollDownToUp:
            return vertical_drop_rc_track_right_barrel_roll_down_to_up;
        case TrackElemType::PoweredLift:
            return vertical_drop_rc_track_powered_lift;
        case TrackElemType::LeftLargeHalfLoopUp:
            return vertical_drop_rc_track_left_large_half_loop_up;
        case TrackElemType::RightLargeHalfLoopUp:
            return vertical_drop_rc_track_right_large_half_loop_up;
        case TrackElemType::RightLargeHalfLoopDown:
            return vertical_drop_rc_track_right_large_half_loop_down;
        case TrackElemType::LeftLargeHalfLoopDown:
            return vertical_drop_rc_track_left_large_half_loop_down;
        case TrackElemType::Up90ToInvertedFlatQuarterLoop:
            return vertical_drop_rc_track_90_deg_to_inverted_flat_quarter_loop_up;
        case TrackElemType::InvertedFlatToDown90QuarterLoop:
            return vertical_drop_rc_track_inverted_flat_to_90_deg_quarter_loop_down;

        case TrackElemType::Booster:
            return vertical_drop_rc_track_booster;
    }
    return nullptr;
}
