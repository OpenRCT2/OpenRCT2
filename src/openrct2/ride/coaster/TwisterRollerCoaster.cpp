/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.TileElement.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../../world/Sprite.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"
#include "BolligerMabillardTrack.h"

/** rct2: 0x008AB6A4 */
static void twister_rc_track_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8F4, 0x008AB904, 0x008AB914 */
static void twister_rc_track_station(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_station(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6B4 */
static void twister_rc_track_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6C4 */
static void twister_rc_track_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6D4 */
static void twister_rc_track_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6E4 */
static void twister_rc_track_25_deg_up_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6F4 */
static void twister_rc_track_60_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB704 */
static void twister_rc_track_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB714 */
static void twister_rc_track_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB724 */
static void twister_rc_track_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB734 */
static void twister_rc_track_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB744 */
static void twister_rc_track_25_deg_down_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB754 */
static void twister_rc_track_60_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_down_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB764 */
static void twister_rc_track_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB774 */
static void twister_rc_track_left_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_5(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB784 */
static void twister_rc_track_right_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_5(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB794 */
static void twister_rc_track_flat_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7A4 */
static void twister_rc_track_flat_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7B4 */
static void twister_rc_track_left_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_bank_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7C4 */
static void twister_rc_track_right_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_bank_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7D4 */
static void twister_rc_track_banked_left_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_banked_left_quarter_turn_5(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7E4 */
static void twister_rc_track_banked_right_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_banked_right_quarter_turn_5(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7F4 */
static void twister_rc_track_left_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_bank_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB804 */
static void twister_rc_track_right_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_bank_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB814 */
static void twister_rc_track_25_deg_up_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB824 */
static void twister_rc_track_25_deg_up_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB834 */
static void twister_rc_track_left_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_bank_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB844 */
static void twister_rc_track_right_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_bank_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB854 */
static void twister_rc_track_25_deg_down_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB864 */
static void twister_rc_track_25_deg_down_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB874 */
static void twister_rc_track_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_bank(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB884 */
static void twister_rc_track_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB894 */
static void twister_rc_track_left_quarter_turn_5_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8A4 */
static void twister_rc_track_right_quarter_turn_5_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8B4 */
static void twister_rc_track_left_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_5_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8C4 */
static void twister_rc_track_right_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_5_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8D4 */
static void twister_rc_track_s_bend_left(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_s_bend_left(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8E4 */
static void twister_rc_track_s_bend_right(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_s_bend_right(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA84 */
static void twister_rc_track_left_vertical_loop(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_vertical_loop(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA94 */
static void twister_rc_track_right_vertical_loop(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_vertical_loop(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB924 */
static void twister_rc_track_left_quarter_turn_3(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_3(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB934 */
static void twister_rc_track_right_quarter_turn_3(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_3(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB944 */
static void twister_rc_track_left_quarter_turn_3_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB954 */
static void twister_rc_track_right_quarter_turn_3_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB964 */
static void twister_rc_track_left_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB974 */
static void twister_rc_track_right_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB984 */
static void twister_rc_track_left_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB994 */
static void twister_rc_track_right_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9A4 */
static void twister_rc_track_left_half_banked_helix_up_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_half_banked_helix_up_small(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9B4 */
static void twister_rc_track_right_half_banked_helix_up_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_half_banked_helix_up_small(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9C4 */
static void twister_rc_track_left_half_banked_helix_down_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_half_banked_helix_down_small(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9D4 */
static void twister_rc_track_right_half_banked_helix_down_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_half_banked_helix_down_small(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9E4 */
static void twister_rc_track_left_half_banked_helix_up_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_half_banked_helix_up_large(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9F4 */
static void twister_rc_track_right_half_banked_helix_up_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_half_banked_helix_up_large(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA04 */
static void twister_rc_track_left_half_banked_helix_down_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_half_banked_helix_down_large(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA14 */
static void twister_rc_track_right_half_banked_helix_down_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_half_banked_helix_down_large(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA44 */
static void twister_rc_track_left_quarter_turn_1_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA24 */
static void twister_rc_track_right_quarter_turn_1_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA34 */
static void twister_rc_track_left_quarter_turn_1_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA54 */
static void twister_rc_track_right_quarter_turn_1_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA64 */
static void twister_rc_track_brakes(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_brakes(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE04 */
static void twister_rc_track_25_deg_up_left_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_left_banked(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE14 */
static void twister_rc_track_25_deg_up_right_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_right_banked(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA74 */
static void twister_rc_track_on_ride_photo(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_on_ride_photo(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE24 */
static void twister_rc_track_25_deg_down_left_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_left_banked(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE34 */
static void twister_rc_track_25_deg_down_right_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_right_banked(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE44 */
static void twister_rc_track_90_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_90_deg_up(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE54 */
static void twister_rc_track_90_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_90_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE64 */
static void twister_rc_track_60_deg_up_to_90_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up_to_90_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE74 */
static void twister_rc_track_90_deg_down_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_90_deg_down_to_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE84 */
static void twister_rc_track_90_deg_up_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_90_deg_up_to_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE94 */
static void twister_rc_track_60_deg_down_to_90_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_down_to_90_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAB4 */
static void twister_rc_track_left_eighth_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_eighth_to_diag(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAC4 */
static void twister_rc_track_right_eighth_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_eighth_to_diag(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAD4 */
static void twister_rc_track_left_eighth_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_eighth_to_orthogonal(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAE4 */
static void twister_rc_track_right_eighth_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_eighth_to_orthogonal(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAF4 */
static void twister_rc_track_left_eighth_bank_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_eighth_bank_to_diag(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB04 */
static void twister_rc_track_right_eighth_bank_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_eighth_bank_to_diag(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB14 */
static void twister_rc_track_left_eighth_bank_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_eighth_bank_to_orthogonal(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB24 */
static void twister_rc_track_right_eighth_bank_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_eighth_bank_to_orthogonal(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAA4 */
static void twister_rc_track_diag_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB54 */
static void twister_rc_track_diag_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBB4 */
static void twister_rc_track_diag_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB34 */
static void twister_rc_track_diag_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB94 */
static void twister_rc_track_diag_25_deg_up_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBA4 */
static void twister_rc_track_diag_60_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB44 */
static void twister_rc_track_diag_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB84 */
static void twister_rc_track_diag_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBE4 */
static void twister_rc_track_diag_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB64 */
static void twister_rc_track_diag_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBC4 */
static void twister_rc_track_diag_25_deg_down_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBD4 */
static void twister_rc_track_diag_60_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_down_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB74 */
static void twister_rc_track_diag_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC14 */
static void twister_rc_track_diag_flat_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC24 */
static void twister_rc_track_diag_flat_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC34 */
static void twister_rc_track_diag_left_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_left_bank_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC44 */
static void twister_rc_track_diag_right_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_right_bank_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC74 */
static void twister_rc_track_diag_left_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_left_bank_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC84 */
static void twister_rc_track_diag_right_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_right_bank_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC54 */
static void twister_rc_track_diag_25_deg_up_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC64 */
static void twister_rc_track_diag_25_deg_up_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC94 */
static void twister_rc_track_diag_left_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_left_bank_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCA4 */
static void twister_rc_track_diag_right_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_right_bank_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCB4 */
static void twister_rc_track_diag_25_deg_down_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCC4 */
static void twister_rc_track_diag_25_deg_down_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBF4 */
static void twister_rc_track_diag_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC04 */
static void twister_rc_track_diag_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD74 */
static void twister_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_bank_to_left_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD84 */
static void twister_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_bank_to_right_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD94 */
static void twister_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_down_to_left_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDA4 */
static void twister_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_down_to_right_bank(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0E4 */
static void twister_rc_track_block_brakes(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_block_brakes(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEA4 */
static void twister_rc_track_left_banked_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEB4 */
static void twister_rc_track_right_banked_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEC4 */
static void twister_rc_track_left_banked_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABED4 */
static void twister_rc_track_right_banked_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEE4 */
static void twister_rc_track_left_banked_quarter_turn_5_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEF4 */
static void twister_rc_track_right_banked_quarter_turn_5_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF04 */
static void twister_rc_track_left_banked_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF14 */
static void twister_rc_track_right_banked_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_5_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF24 */
static void twister_rc_track_25_deg_up_to_left_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_left_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF34 */
static void twister_rc_track_25_deg_up_to_right_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_up_to_right_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF44 */
static void twister_rc_track_left_banked_25_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF54 */
static void twister_rc_track_right_banked_25_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF64 */
static void twister_rc_track_25_deg_down_to_left_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_left_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF74 */
static void twister_rc_track_25_deg_down_to_right_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_25_deg_down_to_right_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF84 */
static void twister_rc_track_left_banked_25_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF94 */
static void twister_rc_track_right_banked_25_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFA4 */
static void twister_rc_track_left_banked_flat_to_left_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_flat_to_left_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFB4 */
static void twister_rc_track_right_banked_flat_to_right_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_flat_to_right_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFE4 */
static void twister_rc_track_left_banked_25_deg_up_to_left_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_left_banked_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFF4 */
static void twister_rc_track_right_banked_25_deg_up_to_right_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_right_banked_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC004 */
static void twister_rc_track_left_banked_flat_to_left_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_flat_to_left_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC014 */
static void twister_rc_track_right_banked_flat_to_right_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_flat_to_right_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFC4 */
static void twister_rc_track_left_banked_25_deg_down_to_left_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_left_banked_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFD4 */
static void twister_rc_track_right_banked_25_deg_down_to_right_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_right_banked_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC024 */
static void twister_rc_track_flat_to_left_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_left_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC034 */
static void twister_rc_track_flat_to_right_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_right_banked_25_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC044 */
static void twister_rc_track_left_banked_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC054 */
static void twister_rc_track_right_banked_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC064 */
static void twister_rc_track_flat_to_left_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_left_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC074 */
static void twister_rc_track_flat_to_right_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_right_banked_25_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC084 */
static void twister_rc_track_left_banked_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC094 */
static void twister_rc_track_right_banked_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0A4 */
static void twister_rc_track_left_quarter_turn_1_90_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_90_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0B4 */
static void twister_rc_track_right_quarter_turn_1_90_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_90_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0C4 */
static void twister_rc_track_left_quarter_turn_1_90_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_90_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0D4 */
static void twister_rc_track_right_quarter_turn_1_90_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_90_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/* The following track elements used to be specific to the Vertical Roller Coaster */
static void twister_rc_track_flat_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_60_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_flat_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_60_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_brake_for_drop(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_brake_for_drop(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_diag_flat_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_60_deg_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_diag_60_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_up_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_diag_flat_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_flat_to_60_deg_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_diag_60_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_diag_60_deg_down_to_flat(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/* The following track elements used to be specific to the Steel Twister */
/** rct2: 0x008ABCD4 */
static void twister_rc_track_half_loop_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_half_loop_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCE4 */
static void twister_rc_track_half_loop_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_half_loop_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD34 */
static void twister_rc_track_left_corkscrew_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_corkscrew_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD44 */
static void twister_rc_track_right_corkscrew_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_corkscrew_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD54 */
static void twister_rc_track_left_corkscrew_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_corkscrew_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD64 */
static void twister_rc_track_right_corkscrew_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_corkscrew_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0F4 */
static void twister_rc_track_flat_to_60_deg_up_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up_long_base(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC104 */
static void twister_rc_track_60_deg_up_to_flat_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat_long_base(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC114 */
static void twister_rc_track_60_deg_down_to_flat_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_flat_to_60_deg_down_long_base(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC124 */
static void twister_rc_track_flat_to_60_deg_down_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat_long_base122(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCF4 */
static void twister_rc_track_left_barrel_roll_up_to_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_barrel_roll_up_to_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD04 */
static void twister_rc_track_right_barrel_roll_up_to_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_barrel_roll_up_to_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD14 */
static void twister_rc_track_left_barrel_roll_down_to_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_barrel_roll_down_to_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD24 */
static void twister_rc_track_right_barrel_roll_down_to_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_barrel_roll_down_to_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDB4 */
static void twister_rc_track_powered_lift(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_powered_lift(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDC4 */
static void twister_rc_track_left_large_half_loop_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_large_half_loop_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDD4 */
static void twister_rc_track_right_large_half_loop_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_large_half_loop_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDE4 */
static void twister_rc_track_right_large_half_loop_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_right_large_half_loop_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDF4 */
static void twister_rc_track_left_large_half_loop_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_left_large_half_loop_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC134 */
static void twister_rc_track_90_deg_to_inverted_flat_quarter_loop_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_90_deg_to_inverted_flat_quarter_loop_up(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC144 */
static void twister_rc_track_inverted_flat_to_90_deg_quarter_loop_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_inverted_flat_to_90_deg_quarter_loop_down(
        session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_booster(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    bolliger_mabillard_track_booster(session, rideIndex, trackSequence, direction, height, tileElement, METAL_SUPPORTS_TUBES);
}

TRACK_PAINT_FUNCTION get_track_paint_function_twister_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return twister_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return twister_rc_track_station;
        case TrackElemType::Up25:
            return twister_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return twister_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return twister_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return twister_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return twister_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return twister_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return twister_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return twister_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return twister_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return twister_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return twister_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return twister_rc_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return twister_rc_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return twister_rc_track_right_quarter_turn_5;
        case TrackElemType::FlatToLeftBank:
            return twister_rc_track_flat_to_left_bank;
        case TrackElemType::FlatToRightBank:
            return twister_rc_track_flat_to_right_bank;
        case TrackElemType::LeftBankToFlat:
            return twister_rc_track_left_bank_to_flat;
        case TrackElemType::RightBankToFlat:
            return twister_rc_track_right_bank_to_flat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return twister_rc_track_banked_left_quarter_turn_5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return twister_rc_track_banked_right_quarter_turn_5;
        case TrackElemType::LeftBankToUp25:
            return twister_rc_track_left_bank_to_25_deg_up;
        case TrackElemType::RightBankToUp25:
            return twister_rc_track_right_bank_to_25_deg_up;
        case TrackElemType::Up25ToLeftBank:
            return twister_rc_track_25_deg_up_to_left_bank;
        case TrackElemType::Up25ToRightBank:
            return twister_rc_track_25_deg_up_to_right_bank;
        case TrackElemType::LeftBankToDown25:
            return twister_rc_track_left_bank_to_25_deg_down;
        case TrackElemType::RightBankToDown25:
            return twister_rc_track_right_bank_to_25_deg_down;
        case TrackElemType::Down25ToLeftBank:
            return twister_rc_track_25_deg_down_to_left_bank;
        case TrackElemType::Down25ToRightBank:
            return twister_rc_track_25_deg_down_to_right_bank;
        case TrackElemType::LeftBank:
            return twister_rc_track_left_bank;
        case TrackElemType::RightBank:
            return twister_rc_track_right_bank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return twister_rc_track_left_quarter_turn_5_25_deg_up;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return twister_rc_track_right_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return twister_rc_track_left_quarter_turn_5_25_deg_down;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return twister_rc_track_right_quarter_turn_5_25_deg_down;
        case TrackElemType::SBendLeft:
            return twister_rc_track_s_bend_left;
        case TrackElemType::SBendRight:
            return twister_rc_track_s_bend_right;
        case TrackElemType::LeftVerticalLoop:
            return twister_rc_track_left_vertical_loop;
        case TrackElemType::RightVerticalLoop:
            return twister_rc_track_right_vertical_loop;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return twister_rc_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return twister_rc_track_right_quarter_turn_3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return twister_rc_track_left_quarter_turn_3_bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return twister_rc_track_right_quarter_turn_3_bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return twister_rc_track_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return twister_rc_track_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return twister_rc_track_left_quarter_turn_3_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return twister_rc_track_right_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return twister_rc_track_left_half_banked_helix_up_small;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return twister_rc_track_right_half_banked_helix_up_small;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return twister_rc_track_left_half_banked_helix_down_small;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return twister_rc_track_right_half_banked_helix_down_small;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return twister_rc_track_left_half_banked_helix_up_large;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return twister_rc_track_right_half_banked_helix_up_large;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return twister_rc_track_left_half_banked_helix_down_large;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return twister_rc_track_right_half_banked_helix_down_large;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return twister_rc_track_left_quarter_turn_1_60_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return twister_rc_track_right_quarter_turn_1_60_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return twister_rc_track_left_quarter_turn_1_60_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return twister_rc_track_right_quarter_turn_1_60_deg_down;
        case TrackElemType::Brakes:
            return twister_rc_track_brakes;
        case TrackElemType::Up25LeftBanked:
            return twister_rc_track_25_deg_up_left_banked;
        case TrackElemType::Up25RightBanked:
            return twister_rc_track_25_deg_up_right_banked;
        case TrackElemType::OnRidePhoto:
            return twister_rc_track_on_ride_photo;
        case TrackElemType::Down25LeftBanked:
            return twister_rc_track_25_deg_down_left_banked;
        case TrackElemType::Down25RightBanked:
            return twister_rc_track_25_deg_down_right_banked;
        case TrackElemType::Up90:
            return twister_rc_track_90_deg_up;
        case TrackElemType::Down90:
            return twister_rc_track_90_deg_down;
        case TrackElemType::Up60ToUp90:
            return twister_rc_track_60_deg_up_to_90_deg_up;
        case TrackElemType::Down90ToDown60:
            return twister_rc_track_90_deg_down_to_60_deg_down;
        case TrackElemType::Up90ToUp60:
            return twister_rc_track_90_deg_up_to_60_deg_up;
        case TrackElemType::Down60ToDown90:
            return twister_rc_track_60_deg_down_to_90_deg_down;
        case TrackElemType::LeftEighthToDiag:
            return twister_rc_track_left_eighth_to_diag;
        case TrackElemType::RightEighthToDiag:
            return twister_rc_track_right_eighth_to_diag;
        case TrackElemType::LeftEighthToOrthogonal:
            return twister_rc_track_left_eighth_to_orthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return twister_rc_track_right_eighth_to_orthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return twister_rc_track_left_eighth_bank_to_diag;
        case TrackElemType::RightEighthBankToDiag:
            return twister_rc_track_right_eighth_bank_to_diag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return twister_rc_track_left_eighth_bank_to_orthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return twister_rc_track_right_eighth_bank_to_orthogonal;
        case TrackElemType::DiagFlat:
            return twister_rc_track_diag_flat;
        case TrackElemType::DiagUp25:
            return twister_rc_track_diag_25_deg_up;
        case TrackElemType::DiagUp60:
            return twister_rc_track_diag_60_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return twister_rc_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToUp60:
            return twister_rc_track_diag_25_deg_up_to_60_deg_up;
        case TrackElemType::DiagUp60ToUp25:
            return twister_rc_track_diag_60_deg_up_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return twister_rc_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return twister_rc_track_diag_25_deg_down;
        case TrackElemType::DiagDown60:
            return twister_rc_track_diag_60_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return twister_rc_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToDown60:
            return twister_rc_track_diag_25_deg_down_to_60_deg_down;
        case TrackElemType::DiagDown60ToDown25:
            return twister_rc_track_diag_60_deg_down_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return twister_rc_track_diag_25_deg_down_to_flat;
        case TrackElemType::DiagFlatToLeftBank:
            return twister_rc_track_diag_flat_to_left_bank;
        case TrackElemType::DiagFlatToRightBank:
            return twister_rc_track_diag_flat_to_right_bank;
        case TrackElemType::DiagLeftBankToFlat:
            return twister_rc_track_diag_left_bank_to_flat;
        case TrackElemType::DiagRightBankToFlat:
            return twister_rc_track_diag_right_bank_to_flat;
        case TrackElemType::DiagLeftBankToUp25:
            return twister_rc_track_diag_left_bank_to_25_deg_up;
        case TrackElemType::DiagRightBankToUp25:
            return twister_rc_track_diag_right_bank_to_25_deg_up;
        case TrackElemType::DiagUp25ToLeftBank:
            return twister_rc_track_diag_25_deg_up_to_left_bank;
        case TrackElemType::DiagUp25ToRightBank:
            return twister_rc_track_diag_25_deg_up_to_right_bank;
        case TrackElemType::DiagLeftBankToDown25:
            return twister_rc_track_diag_left_bank_to_25_deg_down;
        case TrackElemType::DiagRightBankToDown25:
            return twister_rc_track_diag_right_bank_to_25_deg_down;
        case TrackElemType::DiagDown25ToLeftBank:
            return twister_rc_track_diag_25_deg_down_to_left_bank;
        case TrackElemType::DiagDown25ToRightBank:
            return twister_rc_track_diag_25_deg_down_to_right_bank;
        case TrackElemType::DiagLeftBank:
            return twister_rc_track_diag_left_bank;
        case TrackElemType::DiagRightBank:
            return twister_rc_track_diag_right_bank;
        case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
            return twister_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
            return twister_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
            return twister_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank;
        case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
            return twister_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank;
        case TrackElemType::BlockBrakes:
            return twister_rc_track_block_brakes;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
            return twister_rc_track_left_banked_quarter_turn_3_25_deg_up;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
            return twister_rc_track_right_banked_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return twister_rc_track_left_banked_quarter_turn_3_25_deg_down;
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return twister_rc_track_right_banked_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
            return twister_rc_track_left_banked_quarter_turn_5_25_deg_up;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
            return twister_rc_track_right_banked_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return twister_rc_track_left_banked_quarter_turn_5_25_deg_down;
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return twister_rc_track_right_banked_quarter_turn_5_25_deg_down;
        case TrackElemType::Up25ToLeftBankedUp25:
            return twister_rc_track_25_deg_up_to_left_banked_25_deg_up;
        case TrackElemType::Up25ToRightBankedUp25:
            return twister_rc_track_25_deg_up_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToUp25:
            return twister_rc_track_left_banked_25_deg_up_to_25_deg_up;
        case TrackElemType::RightBankedUp25ToUp25:
            return twister_rc_track_right_banked_25_deg_up_to_25_deg_up;
        case TrackElemType::Down25ToLeftBankedDown25:
            return twister_rc_track_25_deg_down_to_left_banked_25_deg_down;
        case TrackElemType::Down25ToRightBankedDown25:
            return twister_rc_track_25_deg_down_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToDown25:
            return twister_rc_track_left_banked_25_deg_down_to_25_deg_down;
        case TrackElemType::RightBankedDown25ToDown25:
            return twister_rc_track_right_banked_25_deg_down_to_25_deg_down;
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
            return twister_rc_track_left_banked_flat_to_left_banked_25_deg_up;
        case TrackElemType::RightBankedFlatToRightBankedUp25:
            return twister_rc_track_right_banked_flat_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
            return twister_rc_track_left_banked_25_deg_up_to_left_banked_flat;
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
            return twister_rc_track_right_banked_25_deg_up_to_right_banked_flat;
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
            return twister_rc_track_left_banked_flat_to_left_banked_25_deg_down;
        case TrackElemType::RightBankedFlatToRightBankedDown25:
            return twister_rc_track_right_banked_flat_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
            return twister_rc_track_left_banked_25_deg_down_to_left_banked_flat;
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
            return twister_rc_track_right_banked_25_deg_down_to_right_banked_flat;
        case TrackElemType::FlatToLeftBankedUp25:
            return twister_rc_track_flat_to_left_banked_25_deg_up;
        case TrackElemType::FlatToRightBankedUp25:
            return twister_rc_track_flat_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToFlat:
            return twister_rc_track_left_banked_25_deg_up_to_flat;
        case TrackElemType::RightBankedUp25ToFlat:
            return twister_rc_track_right_banked_25_deg_up_to_flat;
        case TrackElemType::FlatToLeftBankedDown25:
            return twister_rc_track_flat_to_left_banked_25_deg_down;
        case TrackElemType::FlatToRightBankedDown25:
            return twister_rc_track_flat_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToFlat:
            return twister_rc_track_left_banked_25_deg_down_to_flat;
        case TrackElemType::RightBankedDown25ToFlat:
            return twister_rc_track_right_banked_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn1TileUp90:
            return twister_rc_track_left_quarter_turn_1_90_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp90:
            return twister_rc_track_right_quarter_turn_1_90_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown90:
            return twister_rc_track_left_quarter_turn_1_90_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown90:
            return twister_rc_track_right_quarter_turn_1_90_deg_down;
        /* The following track elements used to be specific to the vertical RC */
        case TrackElemType::FlatToUp60:
            return twister_rc_track_flat_to_60_deg_up;
        case TrackElemType::Up60ToFlat:
            return twister_rc_track_60_deg_up_to_flat;
        case TrackElemType::FlatToDown60:
            return twister_rc_track_flat_to_60_deg_down;
        case TrackElemType::Down60ToFlat:
            return twister_rc_track_60_deg_down_to_flat;
        case TrackElemType::BrakeForDrop:
            return twister_rc_track_brake_for_drop;
        case TrackElemType::DiagFlatToUp60:
            return twister_rc_track_diag_flat_to_60_deg_up;
        case TrackElemType::DiagUp60ToFlat:
            return twister_rc_track_diag_60_deg_up_to_flat;
        case TrackElemType::DiagFlatToDown60:
            return twister_rc_track_diag_flat_to_60_deg_down;
        case TrackElemType::DiagDown60ToFlat:
            return twister_rc_track_diag_60_deg_down_to_flat;
        /* The following track elements used to be specific to the Twister RC */
        case TrackElemType::HalfLoopUp:
            return twister_rc_track_half_loop_up;
        case TrackElemType::HalfLoopDown:
            return twister_rc_track_half_loop_down;
        case TrackElemType::LeftCorkscrewUp:
            return twister_rc_track_left_corkscrew_up;
        case TrackElemType::RightCorkscrewUp:
            return twister_rc_track_right_corkscrew_up;
        case TrackElemType::LeftCorkscrewDown:
            return twister_rc_track_left_corkscrew_down;
        case TrackElemType::RightCorkscrewDown:
            return twister_rc_track_right_corkscrew_down;
        case TrackElemType::FlatToUp60LongBase:
            return twister_rc_track_flat_to_60_deg_up_long_base;
        case TrackElemType::Up60ToFlatLongBase:
            return twister_rc_track_60_deg_up_to_flat_long_base;
        case TrackElemType::Down60ToFlatLongBase:
            return twister_rc_track_60_deg_down_to_flat_long_base;
        case TrackElemType::FlatToDown60LongBase:
            return twister_rc_track_flat_to_60_deg_down_long_base;
        case TrackElemType::LeftBarrelRollUpToDown:
            return twister_rc_track_left_barrel_roll_up_to_down;
        case TrackElemType::RightBarrelRollUpToDown:
            return twister_rc_track_right_barrel_roll_up_to_down;
        case TrackElemType::LeftBarrelRollDownToUp:
            return twister_rc_track_left_barrel_roll_down_to_up;
        case TrackElemType::RightBarrelRollDownToUp:
            return twister_rc_track_right_barrel_roll_down_to_up;
        case TrackElemType::PoweredLift:
            return twister_rc_track_powered_lift;
        case TrackElemType::LeftLargeHalfLoopUp:
            return twister_rc_track_left_large_half_loop_up;
        case TrackElemType::RightLargeHalfLoopUp:
            return twister_rc_track_right_large_half_loop_up;
        case TrackElemType::RightLargeHalfLoopDown:
            return twister_rc_track_right_large_half_loop_down;
        case TrackElemType::LeftLargeHalfLoopDown:
            return twister_rc_track_left_large_half_loop_down;
        case TrackElemType::Up90ToInvertedFlatQuarterLoop:
            return twister_rc_track_90_deg_to_inverted_flat_quarter_loop_up;
        case TrackElemType::InvertedFlatToDown90QuarterLoop:
            return twister_rc_track_inverted_flat_to_90_deg_quarter_loop_down;

        case TrackElemType::Booster:
            return twister_rc_track_booster;
    }
    return nullptr;
}
