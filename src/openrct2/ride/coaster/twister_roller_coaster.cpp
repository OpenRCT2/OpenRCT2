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
#include "bolliger_mabillard_track.h"


/** rct2: 0x008AB6A4 */
static void twister_rc_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8F4, 0x008AB904, 0x008AB914 */
static void twister_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_station(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6B4 */
static void twister_rc_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6C4 */
static void twister_rc_track_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6D4 */
static void twister_rc_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6E4 */
static void twister_rc_track_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6F4 */
static void twister_rc_track_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB704 */
static void twister_rc_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB714 */
static void twister_rc_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB724 */
static void twister_rc_track_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB734 */
static void twister_rc_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB744 */
static void twister_rc_track_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB754 */
static void twister_rc_track_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_down_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB764 */
static void twister_rc_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB774 */
static void twister_rc_track_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_5(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB784 */
static void twister_rc_track_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_5(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB794 */
static void twister_rc_track_flat_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7A4 */
static void twister_rc_track_flat_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7B4 */
static void twister_rc_track_left_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7C4 */
static void twister_rc_track_right_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7D4 */
static void twister_rc_track_banked_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_banked_left_quarter_turn_5(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7E4 */
static void twister_rc_track_banked_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_banked_right_quarter_turn_5(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7F4 */
static void twister_rc_track_left_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB804 */
static void twister_rc_track_right_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB814 */
static void twister_rc_track_25_deg_up_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB824 */
static void twister_rc_track_25_deg_up_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB834 */
static void twister_rc_track_left_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB844 */
static void twister_rc_track_right_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB854 */
static void twister_rc_track_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB864 */
static void twister_rc_track_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB874 */
static void twister_rc_track_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB884 */
static void twister_rc_track_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB894 */
static void twister_rc_track_left_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8A4 */
static void twister_rc_track_right_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8B4 */
static void twister_rc_track_left_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_5_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8C4 */
static void twister_rc_track_right_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_5_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8D4 */
static void twister_rc_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_s_bend_left(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8E4 */
static void twister_rc_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_s_bend_right(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA84 */
static void twister_rc_track_left_vertical_loop(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_vertical_loop(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA94 */
static void twister_rc_track_right_vertical_loop(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_vertical_loop(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB924 */
static void twister_rc_track_left_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB934 */
static void twister_rc_track_right_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB944 */
static void twister_rc_track_left_quarter_turn_3_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB954 */
static void twister_rc_track_right_quarter_turn_3_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB964 */
static void twister_rc_track_left_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB974 */
static void twister_rc_track_right_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB984 */
static void twister_rc_track_left_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB994 */
static void twister_rc_track_right_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9A4 */
static void twister_rc_track_left_half_banked_helix_up_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_half_banked_helix_up_small(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9B4 */
static void twister_rc_track_right_half_banked_helix_up_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_half_banked_helix_up_small(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9C4 */
static void twister_rc_track_left_half_banked_helix_down_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_half_banked_helix_down_small(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9D4 */
static void twister_rc_track_right_half_banked_helix_down_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_half_banked_helix_down_small(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9E4 */
static void twister_rc_track_left_half_banked_helix_up_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_half_banked_helix_up_large(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9F4 */
static void twister_rc_track_right_half_banked_helix_up_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_half_banked_helix_up_large(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA04 */
static void twister_rc_track_left_half_banked_helix_down_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_half_banked_helix_down_large(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA14 */
static void twister_rc_track_right_half_banked_helix_down_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_half_banked_helix_down_large(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA44 */
static void twister_rc_track_left_quarter_turn_1_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA24 */
static void twister_rc_track_right_quarter_turn_1_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA34 */
static void twister_rc_track_left_quarter_turn_1_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA54 */
static void twister_rc_track_right_quarter_turn_1_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA64 */
static void twister_rc_track_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_brakes(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE04 */
static void twister_rc_track_25_deg_up_left_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_left_banked(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE14 */
static void twister_rc_track_25_deg_up_right_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_right_banked(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA74 */
static void twister_rc_track_on_ride_photo(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_on_ride_photo(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE24 */
static void twister_rc_track_25_deg_down_left_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_left_banked(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE34 */
static void twister_rc_track_25_deg_down_right_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_right_banked(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE44 */
static void twister_rc_track_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_90_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE54 */
static void twister_rc_track_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_90_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE64 */
static void twister_rc_track_60_deg_up_to_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_90_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE74 */
static void twister_rc_track_90_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_90_deg_down_to_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE84 */
static void twister_rc_track_90_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_90_deg_up_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE94 */
static void twister_rc_track_60_deg_down_to_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_down_to_90_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAB4 */
static void twister_rc_track_left_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_eighth_to_diag(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAC4 */
static void twister_rc_track_right_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_eighth_to_diag(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAD4 */
static void twister_rc_track_left_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_eighth_to_orthogonal(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAE4 */
static void twister_rc_track_right_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_eighth_to_orthogonal(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAF4 */
static void twister_rc_track_left_eighth_bank_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_eighth_bank_to_diag(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB04 */
static void twister_rc_track_right_eighth_bank_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_eighth_bank_to_diag(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB14 */
static void twister_rc_track_left_eighth_bank_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_eighth_bank_to_orthogonal(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB24 */
static void twister_rc_track_right_eighth_bank_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_eighth_bank_to_orthogonal(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAA4 */
static void twister_rc_track_diag_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB54 */
static void twister_rc_track_diag_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBB4 */
static void twister_rc_track_diag_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB34 */
static void twister_rc_track_diag_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB94 */
static void twister_rc_track_diag_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBA4 */
static void twister_rc_track_diag_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB44 */
static void twister_rc_track_diag_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB84 */
static void twister_rc_track_diag_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBE4 */
static void twister_rc_track_diag_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB64 */
static void twister_rc_track_diag_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBC4 */
static void twister_rc_track_diag_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBD4 */
static void twister_rc_track_diag_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_down_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB74 */
static void twister_rc_track_diag_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC14 */
static void twister_rc_track_diag_flat_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC24 */
static void twister_rc_track_diag_flat_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC34 */
static void twister_rc_track_diag_left_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_left_bank_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC44 */
static void twister_rc_track_diag_right_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_right_bank_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC74 */
static void twister_rc_track_diag_left_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_left_bank_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC84 */
static void twister_rc_track_diag_right_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_right_bank_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC54 */
static void twister_rc_track_diag_25_deg_up_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC64 */
static void twister_rc_track_diag_25_deg_up_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC94 */
static void twister_rc_track_diag_left_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_left_bank_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCA4 */
static void twister_rc_track_diag_right_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_right_bank_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCB4 */
static void twister_rc_track_diag_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCC4 */
static void twister_rc_track_diag_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBF4 */
static void twister_rc_track_diag_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_left_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC04 */
static void twister_rc_track_diag_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_right_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD74 */
static void twister_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_left_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD84 */
static void twister_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_right_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD94 */
static void twister_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_down_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDA4 */
static void twister_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_down_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0E4 */
static void twister_rc_track_block_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_block_brakes(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEA4 */
static void twister_rc_track_left_banked_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEB4 */
static void twister_rc_track_right_banked_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEC4 */
static void twister_rc_track_left_banked_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABED4 */
static void twister_rc_track_right_banked_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEE4 */
static void twister_rc_track_left_banked_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEF4 */
static void twister_rc_track_right_banked_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF04 */
static void twister_rc_track_left_banked_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF14 */
static void twister_rc_track_right_banked_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_5_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF24 */
static void twister_rc_track_25_deg_up_to_left_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_left_banked_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF34 */
static void twister_rc_track_25_deg_up_to_right_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_right_banked_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF44 */
static void twister_rc_track_left_banked_25_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF54 */
static void twister_rc_track_right_banked_25_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF64 */
static void twister_rc_track_25_deg_down_to_left_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_left_banked_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF74 */
static void twister_rc_track_25_deg_down_to_right_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_right_banked_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF84 */
static void twister_rc_track_left_banked_25_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABF94 */
static void twister_rc_track_right_banked_25_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFA4 */
static void twister_rc_track_left_banked_flat_to_left_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_flat_to_left_banked_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFB4 */
static void twister_rc_track_right_banked_flat_to_right_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_flat_to_right_banked_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFE4 */
static void twister_rc_track_left_banked_25_deg_up_to_left_banked_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_left_banked_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFF4 */
static void twister_rc_track_right_banked_25_deg_up_to_right_banked_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_right_banked_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC004 */
static void twister_rc_track_left_banked_flat_to_left_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_flat_to_left_banked_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC014 */
static void twister_rc_track_right_banked_flat_to_right_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_flat_to_right_banked_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFC4 */
static void twister_rc_track_left_banked_25_deg_down_to_left_banked_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_left_banked_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABFD4 */
static void twister_rc_track_right_banked_25_deg_down_to_right_banked_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_right_banked_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC024 */
static void twister_rc_track_flat_to_left_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_left_banked_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC034 */
static void twister_rc_track_flat_to_right_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_right_banked_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC044 */
static void twister_rc_track_left_banked_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC054 */
static void twister_rc_track_right_banked_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC064 */
static void twister_rc_track_flat_to_left_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_left_banked_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC074 */
static void twister_rc_track_flat_to_right_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_right_banked_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC084 */
static void twister_rc_track_left_banked_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC094 */
static void twister_rc_track_right_banked_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0A4 */
static void twister_rc_track_left_quarter_turn_1_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_90_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0B4 */
static void twister_rc_track_right_quarter_turn_1_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_90_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0C4 */
static void twister_rc_track_left_quarter_turn_1_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_90_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0D4 */
static void twister_rc_track_right_quarter_turn_1_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_90_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/* The following track elements used to be specific to the Vertical Roller Coaster */
static void twister_rc_track_flat_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_60_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_flat_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_60_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_brake_for_drop(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_brake_for_drop(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_diag_flat_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_diag_60_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_diag_flat_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_diag_60_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/* The following track elements used to be specific to the Steel Twister */
/** rct2: 0x008ABCD4 */
static void twister_rc_track_half_loop_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_half_loop_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCE4 */
static void twister_rc_track_half_loop_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_half_loop_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD34 */
static void twister_rc_track_left_corkscrew_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_corkscrew_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD44 */
static void twister_rc_track_right_corkscrew_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_corkscrew_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD54 */
static void twister_rc_track_left_corkscrew_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_corkscrew_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD64 */
static void twister_rc_track_right_corkscrew_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_corkscrew_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0F4 */
static void twister_rc_track_flat_to_60_deg_up_long_base(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up_long_base(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC104 */
static void twister_rc_track_60_deg_up_to_flat_long_base(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat_long_base(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC114 */
static void twister_rc_track_flat_to_60_deg_down_long_base(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_60_deg_down_long_base(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC124 */
static void twister_rc_track_60_deg_up_to_flat_long_base122(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat_long_base122(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCF4 */
static void twister_rc_track_left_barrel_roll_up_to_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_barrel_roll_up_to_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD04 */
static void twister_rc_track_right_barrel_roll_up_to_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_barrel_roll_up_to_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD14 */
static void twister_rc_track_left_barrel_roll_down_to_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_barrel_roll_down_to_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD24 */
static void twister_rc_track_right_barrel_roll_down_to_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_barrel_roll_down_to_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDB4 */
static void twister_rc_track_powered_lift(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_powered_lift(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDC4 */
static void twister_rc_track_left_large_half_loop_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_large_half_loop_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDD4 */
static void twister_rc_track_right_large_half_loop_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_large_half_loop_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDE4 */
static void twister_rc_track_right_large_half_loop_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_large_half_loop_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDF4 */
static void twister_rc_track_left_large_half_loop_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_large_half_loop_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC134 */
static void twister_rc_track_90_deg_to_inverted_flat_quarter_loop_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_90_deg_to_inverted_flat_quarter_loop_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC144 */
static void twister_rc_track_inverted_flat_to_90_deg_quarter_loop_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_inverted_flat_to_90_deg_quarter_loop_down(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_booster(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_booster(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}


TRACK_PAINT_FUNCTION get_track_paint_function_twister_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return twister_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return twister_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return twister_rc_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return twister_rc_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return twister_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return twister_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return twister_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return twister_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return twister_rc_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return twister_rc_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return twister_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return twister_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return twister_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return twister_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return twister_rc_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return twister_rc_track_right_quarter_turn_5;
    case TRACK_ELEM_FLAT_TO_LEFT_BANK:
        return twister_rc_track_flat_to_left_bank;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
        return twister_rc_track_flat_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_FLAT:
        return twister_rc_track_left_bank_to_flat;
    case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
        return twister_rc_track_right_bank_to_flat;
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
        return twister_rc_track_banked_left_quarter_turn_5;
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
        return twister_rc_track_banked_right_quarter_turn_5;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
        return twister_rc_track_left_bank_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
        return twister_rc_track_right_bank_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
        return twister_rc_track_25_deg_up_to_left_bank;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
        return twister_rc_track_25_deg_up_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
        return twister_rc_track_left_bank_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
        return twister_rc_track_right_bank_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
        return twister_rc_track_25_deg_down_to_left_bank;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
        return twister_rc_track_25_deg_down_to_right_bank;
    case TRACK_ELEM_LEFT_BANK:
        return twister_rc_track_left_bank;
    case TRACK_ELEM_RIGHT_BANK:
        return twister_rc_track_right_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return twister_rc_track_left_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return twister_rc_track_right_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return twister_rc_track_left_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return twister_rc_track_right_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_S_BEND_LEFT:
        return twister_rc_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return twister_rc_track_s_bend_right;
    case TRACK_ELEM_LEFT_VERTICAL_LOOP:
        return twister_rc_track_left_vertical_loop;
    case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
        return twister_rc_track_right_vertical_loop;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return twister_rc_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return twister_rc_track_right_quarter_turn_3;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
        return twister_rc_track_left_quarter_turn_3_bank;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
        return twister_rc_track_right_quarter_turn_3_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return twister_rc_track_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return twister_rc_track_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return twister_rc_track_left_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return twister_rc_track_right_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
        return twister_rc_track_left_half_banked_helix_up_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
        return twister_rc_track_right_half_banked_helix_up_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
        return twister_rc_track_left_half_banked_helix_down_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
        return twister_rc_track_right_half_banked_helix_down_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
        return twister_rc_track_left_half_banked_helix_up_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
        return twister_rc_track_right_half_banked_helix_up_large;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
        return twister_rc_track_left_half_banked_helix_down_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
        return twister_rc_track_right_half_banked_helix_down_large;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP:
        return twister_rc_track_left_quarter_turn_1_60_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP:
        return twister_rc_track_right_quarter_turn_1_60_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        return twister_rc_track_left_quarter_turn_1_60_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        return twister_rc_track_right_quarter_turn_1_60_deg_down;
    case TRACK_ELEM_BRAKES:
        return twister_rc_track_brakes;
    case TRACK_ELEM_25_DEG_UP_LEFT_BANKED:
        return twister_rc_track_25_deg_up_left_banked;
    case TRACK_ELEM_25_DEG_UP_RIGHT_BANKED:
        return twister_rc_track_25_deg_up_right_banked;
    case TRACK_ELEM_ON_RIDE_PHOTO:
        return twister_rc_track_on_ride_photo;
    case TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED:
        return twister_rc_track_25_deg_down_left_banked;
    case TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED:
        return twister_rc_track_25_deg_down_right_banked;
    case TRACK_ELEM_90_DEG_UP:
        return twister_rc_track_90_deg_up;
    case TRACK_ELEM_90_DEG_DOWN:
        return twister_rc_track_90_deg_down;
    case TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP:
        return twister_rc_track_60_deg_up_to_90_deg_up;
    case TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN:
        return twister_rc_track_90_deg_down_to_60_deg_down;
    case TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP:
        return twister_rc_track_90_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN:
        return twister_rc_track_60_deg_down_to_90_deg_down;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
        return twister_rc_track_left_eighth_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
        return twister_rc_track_right_eighth_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        return twister_rc_track_left_eighth_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        return twister_rc_track_right_eighth_to_orthogonal;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
        return twister_rc_track_left_eighth_bank_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
        return twister_rc_track_right_eighth_bank_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
        return twister_rc_track_left_eighth_bank_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
        return twister_rc_track_right_eighth_bank_to_orthogonal;
    case TRACK_ELEM_DIAG_FLAT:
        return twister_rc_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return twister_rc_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP:
        return twister_rc_track_diag_60_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return twister_rc_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
        return twister_rc_track_diag_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
        return twister_rc_track_diag_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return twister_rc_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return twister_rc_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN:
        return twister_rc_track_diag_60_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return twister_rc_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
        return twister_rc_track_diag_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
        return twister_rc_track_diag_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return twister_rc_track_diag_25_deg_down_to_flat;
    case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
        return twister_rc_track_diag_flat_to_left_bank;
    case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
        return twister_rc_track_diag_flat_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
        return twister_rc_track_diag_left_bank_to_flat;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
        return twister_rc_track_diag_right_bank_to_flat;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
        return twister_rc_track_diag_left_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
        return twister_rc_track_diag_right_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
        return twister_rc_track_diag_25_deg_up_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
        return twister_rc_track_diag_25_deg_up_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
        return twister_rc_track_diag_left_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
        return twister_rc_track_diag_right_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
        return twister_rc_track_diag_25_deg_down_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
        return twister_rc_track_diag_25_deg_down_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK:
        return twister_rc_track_diag_left_bank;
    case TRACK_ELEM_DIAG_RIGHT_BANK:
        return twister_rc_track_diag_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return twister_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return twister_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK:
        return twister_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK:
        return twister_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank;
    case TRACK_ELEM_BLOCK_BRAKES:
        return twister_rc_track_block_brakes;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
        return twister_rc_track_left_banked_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
        return twister_rc_track_right_banked_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        return twister_rc_track_left_banked_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        return twister_rc_track_right_banked_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
        return twister_rc_track_left_banked_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
        return twister_rc_track_right_banked_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        return twister_rc_track_left_banked_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        return twister_rc_track_right_banked_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP:
        return twister_rc_track_25_deg_up_to_left_banked_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP:
        return twister_rc_track_25_deg_up_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP:
        return twister_rc_track_left_banked_25_deg_up_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP:
        return twister_rc_track_right_banked_25_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN:
        return twister_rc_track_25_deg_down_to_left_banked_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN:
        return twister_rc_track_25_deg_down_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
        return twister_rc_track_left_banked_25_deg_down_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
        return twister_rc_track_right_banked_25_deg_down_to_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP:
        return twister_rc_track_left_banked_flat_to_left_banked_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
        return twister_rc_track_right_banked_flat_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT:
        return twister_rc_track_left_banked_25_deg_up_to_left_banked_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT:
        return twister_rc_track_right_banked_25_deg_up_to_right_banked_flat;
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
        return twister_rc_track_left_banked_flat_to_left_banked_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
        return twister_rc_track_right_banked_flat_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT:
        return twister_rc_track_left_banked_25_deg_down_to_left_banked_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT:
        return twister_rc_track_right_banked_25_deg_down_to_right_banked_flat;
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP:
        return twister_rc_track_flat_to_left_banked_25_deg_up;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
        return twister_rc_track_flat_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT:
        return twister_rc_track_left_banked_25_deg_up_to_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT:
        return twister_rc_track_right_banked_25_deg_up_to_flat;
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
        return twister_rc_track_flat_to_left_banked_25_deg_down;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
        return twister_rc_track_flat_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT:
        return twister_rc_track_left_banked_25_deg_down_to_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT:
        return twister_rc_track_right_banked_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP:
        return twister_rc_track_left_quarter_turn_1_90_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP:
        return twister_rc_track_right_quarter_turn_1_90_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
        return twister_rc_track_left_quarter_turn_1_90_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
        return twister_rc_track_right_quarter_turn_1_90_deg_down;
    /* The following track elements used to be specific to the vertical RC */
    case TRACK_ELEM_FLAT_TO_60_DEG_UP:
        return twister_rc_track_flat_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_FLAT:
        return twister_rc_track_60_deg_up_to_flat;
    case TRACK_ELEM_FLAT_TO_60_DEG_DOWN:
        return twister_rc_track_flat_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_FLAT:
        return twister_rc_track_60_deg_down_to_flat;
    case TRACK_ELEM_BRAKE_FOR_DROP:
        return twister_rc_track_brake_for_drop;
    case TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP:
        return twister_rc_track_diag_flat_to_60_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
        return twister_rc_track_diag_60_deg_up_to_flat;
    case TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN:
        return twister_rc_track_diag_flat_to_60_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT:
        return twister_rc_track_diag_60_deg_down_to_flat;
    /* The following track elements used to be specific to the Twister RC */
    case TRACK_ELEM_HALF_LOOP_UP:
        return twister_rc_track_half_loop_up;
    case TRACK_ELEM_HALF_LOOP_DOWN:
        return twister_rc_track_half_loop_down;
    case TRACK_ELEM_LEFT_CORKSCREW_UP:
        return twister_rc_track_left_corkscrew_up;
    case TRACK_ELEM_RIGHT_CORKSCREW_UP:
        return twister_rc_track_right_corkscrew_up;
    case TRACK_ELEM_LEFT_CORKSCREW_DOWN:
        return twister_rc_track_left_corkscrew_down;
    case TRACK_ELEM_RIGHT_CORKSCREW_DOWN:
        return twister_rc_track_right_corkscrew_down;
    case TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE:
        return twister_rc_track_flat_to_60_deg_up_long_base;
    case TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE:
        return twister_rc_track_60_deg_up_to_flat_long_base;
    case TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE:
        return twister_rc_track_flat_to_60_deg_down_long_base;
    case TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122:
        return twister_rc_track_60_deg_up_to_flat_long_base122;
    case TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN:
        return twister_rc_track_left_barrel_roll_up_to_down;
    case TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN:
        return twister_rc_track_right_barrel_roll_up_to_down;
    case TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP:
        return twister_rc_track_left_barrel_roll_down_to_up;
    case TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP:
        return twister_rc_track_right_barrel_roll_down_to_up;
    case TRACK_ELEM_POWERED_LIFT:
        return twister_rc_track_powered_lift;
    case TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP:
        return twister_rc_track_left_large_half_loop_up;
    case TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP:
        return twister_rc_track_right_large_half_loop_up;
    case TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN:
        return twister_rc_track_right_large_half_loop_down;
    case TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN:
        return twister_rc_track_left_large_half_loop_down;
    case TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP:
        return twister_rc_track_90_deg_to_inverted_flat_quarter_loop_up;
    case TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN:
        return twister_rc_track_inverted_flat_to_90_deg_quarter_loop_down;

    case TRACK_ELEM_BOOSTER:
        return twister_rc_track_booster;
    }
    return NULL;
}
