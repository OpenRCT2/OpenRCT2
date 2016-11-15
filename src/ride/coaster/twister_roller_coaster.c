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
#include "bolliger_mabillard_track.h"


/** rct2: 0x008AB6A4 */
static void twister_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8F4, 0x008AB904, 0x008AB914 */
static void twister_rc_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_station(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6B4 */
static void twister_rc_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6C4 */
static void twister_rc_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6D4 */
static void twister_rc_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6E4 */
static void twister_rc_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_60_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB6F4 */
static void twister_rc_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB704 */
static void twister_rc_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB714 */
static void twister_rc_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB724 */
static void twister_rc_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB734 */
static void twister_rc_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB744 */
static void twister_rc_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_60_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB754 */
static void twister_rc_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_down_to_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB764 */
static void twister_rc_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB774 */
static void twister_rc_track_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_5(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB784 */
static void twister_rc_track_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_5(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB794 */
static void twister_rc_track_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_left_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7A4 */
static void twister_rc_track_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_right_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7B4 */
static void twister_rc_track_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7C4 */
static void twister_rc_track_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7D4 */
static void twister_rc_track_banked_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_banked_left_quarter_turn_5(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7E4 */
static void twister_rc_track_banked_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_banked_right_quarter_turn_5(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB7F4 */
static void twister_rc_track_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB804 */
static void twister_rc_track_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB814 */
static void twister_rc_track_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_left_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB824 */
static void twister_rc_track_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_right_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB834 */
static void twister_rc_track_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB844 */
static void twister_rc_track_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB854 */
static void twister_rc_track_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_left_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB864 */
static void twister_rc_track_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_right_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB874 */
static void twister_rc_track_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB884 */
static void twister_rc_track_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB894 */
static void twister_rc_track_left_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_5_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8A4 */
static void twister_rc_track_right_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_5_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8B4 */
static void twister_rc_track_left_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_5_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8C4 */
static void twister_rc_track_right_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_5_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8D4 */
static void twister_rc_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_s_bend_left(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB8E4 */
static void twister_rc_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_s_bend_right(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA84 */
static void twister_rc_track_left_vertical_loop(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_vertical_loop(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA94 */
static void twister_rc_track_right_vertical_loop(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_vertical_loop(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB924 */
static void twister_rc_track_left_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB934 */
static void twister_rc_track_right_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB944 */
static void twister_rc_track_left_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB954 */
static void twister_rc_track_right_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB964 */
static void twister_rc_track_left_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB974 */
static void twister_rc_track_right_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB984 */
static void twister_rc_track_left_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB994 */
static void twister_rc_track_right_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9A4 */
static void twister_rc_track_left_half_banked_helix_up_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_half_banked_helix_up_small(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9B4 */
static void twister_rc_track_right_half_banked_helix_up_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_half_banked_helix_up_small(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9C4 */
static void twister_rc_track_left_half_banked_helix_down_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_half_banked_helix_down_small(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9D4 */
static void twister_rc_track_right_half_banked_helix_down_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_half_banked_helix_down_small(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9E4 */
static void twister_rc_track_left_half_banked_helix_up_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_half_banked_helix_up_large(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AB9F4 */
static void twister_rc_track_right_half_banked_helix_up_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_half_banked_helix_up_large(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA04 */
static void twister_rc_track_left_half_banked_helix_down_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_half_banked_helix_down_large(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA14 */
static void twister_rc_track_right_half_banked_helix_down_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_half_banked_helix_down_large(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA44 */
static void twister_rc_track_left_quarter_turn_1_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_quarter_turn_1_60_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA24 */
static void twister_rc_track_right_quarter_turn_1_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_quarter_turn_1_60_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA34 */
static void twister_rc_track_left_quarter_turn_1_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_quarter_turn_1_60_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);}

/** rct2: 0x008ABA54 */
static void twister_rc_track_right_quarter_turn_1_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_quarter_turn_1_60_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);}

/** rct2: 0x008ABA64 */
static void twister_rc_track_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_brakes(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE04 */
static void twister_rc_track_25_deg_up_left_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_25_deg_up_left_banked(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE14 */
static void twister_rc_track_25_deg_up_right_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_25_deg_up_right_banked(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABA74 */
static void twister_rc_track_on_ride_photo(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_on_ride_photo(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE24 */
static void twister_rc_track_25_deg_down_left_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_25_deg_down_left_banked(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE34 */
static void twister_rc_track_25_deg_down_right_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_25_deg_down_right_banked(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE44 */
static void twister_rc_track_90_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_90_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE54 */
static void twister_rc_track_90_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_90_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE64 */
static void twister_rc_track_60_deg_up_to_90_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_60_deg_up_to_90_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE74 */
static void twister_rc_track_90_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_90_deg_down_to_60_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE84 */
static void twister_rc_track_90_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_90_deg_up_to_60_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABE94 */
static void twister_rc_track_60_deg_down_to_90_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_60_deg_down_to_90_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAB4 */
static void twister_rc_track_left_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_eighth_to_diag(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAC4 */
static void twister_rc_track_right_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_eighth_to_diag(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAD4 */
static void twister_rc_track_left_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_eighth_to_orthogonal(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAE4 */
static void twister_rc_track_right_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_eighth_to_orthogonal(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAF4 */
static void twister_rc_track_left_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_eighth_bank_to_diag(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB04 */
static void twister_rc_track_right_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_eighth_bank_to_diag(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB14 */
static void twister_rc_track_left_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_eighth_bank_to_orthogonal(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB24 */
static void twister_rc_track_right_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_eighth_bank_to_orthogonal(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABAA4 */
static void twister_rc_track_diag_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB54 */
static void twister_rc_track_diag_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBB4 */
static void twister_rc_track_diag_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_60_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB34 */
static void twister_rc_track_diag_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_flat_to_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB94 */
static void twister_rc_track_diag_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_25_deg_up_to_60_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBA4 */
static void twister_rc_track_diag_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_60_deg_up_to_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB44 */
static void twister_rc_track_diag_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_25_deg_up_to_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB84 */
static void twister_rc_track_diag_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBE4 */
static void twister_rc_track_diag_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_60_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB64 */
static void twister_rc_track_diag_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_flat_to_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBC4 */
static void twister_rc_track_diag_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_25_deg_down_to_60_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBD4 */
static void twister_rc_track_diag_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_60_deg_down_to_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABB74 */
static void twister_rc_track_diag_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_25_deg_down_to_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC14 */
static void twister_rc_track_diag_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_flat_to_left_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC24 */
static void twister_rc_track_diag_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_flat_to_right_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC34 */
static void twister_rc_track_diag_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_left_bank_to_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC44 */
static void twister_rc_track_diag_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_right_bank_to_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC74 */
static void twister_rc_track_diag_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_left_bank_to_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC84 */
static void twister_rc_track_diag_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_right_bank_to_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC54 */
static void twister_rc_track_diag_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_25_deg_up_to_left_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC64 */
static void twister_rc_track_diag_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_25_deg_up_to_right_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC94 */
static void twister_rc_track_diag_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_left_bank_to_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCA4 */
static void twister_rc_track_diag_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_right_bank_to_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCB4 */
static void twister_rc_track_diag_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_25_deg_down_to_left_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABCC4 */
static void twister_rc_track_diag_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_25_deg_down_to_right_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABBF4 */
static void twister_rc_track_diag_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_left_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABC04 */
static void twister_rc_track_diag_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_diag_right_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD74 */
static void twister_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_bank_to_left_quarter_turn_3_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD84 */
static void twister_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_bank_to_right_quarter_turn_3_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABD94 */
static void twister_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_quarter_turn_3_25_deg_down_to_left_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABDA4 */
static void twister_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_quarter_turn_3_25_deg_down_to_right_bank(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008AC0E4 */
static void twister_rc_track_block_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_block_brakes(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEA4 */
static void twister_rc_track_left_banked_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEB4 */
static void twister_rc_track_right_banked_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEC4 */
static void twister_rc_track_left_banked_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABED4 */
static void twister_rc_track_right_banked_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_down(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEE4 */
static void twister_rc_track_left_banked_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/** rct2: 0x008ABEF4 */
static void twister_rc_track_right_banked_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17958, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17963, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17968, 0, 0, 32, 1, 34, height, 0, 27, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17973, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		}
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		if (direction == 0 || direction == 3) {
			paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17959, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17964, 0, 0, 32, 16, 3, height, 0, 16, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17969, 0, 0, 1, 1, 34, height, 30, 30, height);
			break;
		case 3:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17974, 0, 0, 32, 16, 3, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 3:
		switch (direction) {
		case 0:
			sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17960, 0, 0, 16, 16, 3, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17965, 0, 0, 16, 16, 3, height, 16, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17970, 0, 0, 1, 1, 34, height, 30, 30, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17975, 0, 0, 16, 16, 3, height, 0, 16, height);
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
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17961, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17966, 0, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17971, 0, 0, 1, 1, 34, height, 30, 30, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17976, 0, 0, 16, 32, 3, height, 16, 0, height);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	case 6:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17962, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17967, 0, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17972, 0, 0, 1, 32, 34, height, 27, 0, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17977, 0, 0, 20, 32, 3, height, 6, 0, height);
			break;
		}
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
		switch (direction) {
		case 0:
			paint_util_push_tunnel_right(height + 8, TUNNEL_8);
			break;
		case 1:
			paint_util_push_tunnel_left(height + 8, TUNNEL_8);
			break;
		}
		paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
		paint_util_set_general_support_height(height + 72, 0x20);
		break;
	}
}

/** rct2: 0x008ABF04 */
static void twister_rc_track_left_banked_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	twister_rc_track_right_banked_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008ABF14 */
static void twister_rc_track_right_banked_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	twister_rc_track_left_banked_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008ABF24 */
static void twister_rc_track_25_deg_up_to_left_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17922, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17923, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17930, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17924, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17925, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ABF34 */
static void twister_rc_track_25_deg_up_to_right_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17926, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17927, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17928, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17931, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17929, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ABF44 */
static void twister_rc_track_left_banked_25_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17932, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17933, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17940, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17934, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17935, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ABF54 */
static void twister_rc_track_right_banked_25_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17936, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17937, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17938, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17941, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17939, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ABF64 */
static void twister_rc_track_25_deg_down_to_left_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_right_banked_25_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ABF74 */
static void twister_rc_track_25_deg_down_to_right_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_left_banked_25_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ABF84 */
static void twister_rc_track_left_banked_25_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_25_deg_up_to_right_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ABF94 */
static void twister_rc_track_right_banked_25_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_25_deg_up_to_left_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ABFA4 */
static void twister_rc_track_left_banked_flat_to_left_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17942, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17943, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17944, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17945, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008ABFB4 */
static void twister_rc_track_right_banked_flat_to_right_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17946, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17947, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17948, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17949, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008ABFE4 */
static void twister_rc_track_left_banked_25_deg_up_to_left_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17950, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17951, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17952, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17953, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008ABFF4 */
static void twister_rc_track_right_banked_25_deg_up_to_right_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17954, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17955, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17956, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17957, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008AC004 */
static void twister_rc_track_left_banked_flat_to_left_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_right_banked_25_deg_up_to_right_banked_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC014 */
static void twister_rc_track_right_banked_flat_to_right_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_left_banked_25_deg_up_to_left_banked_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ABFC4 */
static void twister_rc_track_left_banked_25_deg_down_to_left_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_right_banked_flat_to_right_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ABFD4 */
static void twister_rc_track_right_banked_25_deg_down_to_right_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_left_banked_flat_to_left_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC024 */
static void twister_rc_track_flat_to_left_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17894, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17895, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17902, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17896, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17897, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AC034 */
static void twister_rc_track_flat_to_right_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17898, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17899, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17900, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17903, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17901, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AC044 */
static void twister_rc_track_left_banked_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17904, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17905, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17912, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17906, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17907, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008AC054 */
static void twister_rc_track_right_banked_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
	case 0:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17908, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 1:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17909, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	case 2:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17910, 0, 0, 32, 20, 3, height, 0, 6, height);
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17913, 0, 0, 32, 1, 34, height, 0, 27, height);
		break;
	case 3:
		sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17911, 0, 0, 32, 20, 3, height, 0, 6, height);
		break;
	}
	if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
		metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
	}
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008AC064 */
static void twister_rc_track_flat_to_left_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_right_banked_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC074 */
static void twister_rc_track_flat_to_right_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_left_banked_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC084 */
static void twister_rc_track_left_banked_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_flat_to_right_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC094 */
static void twister_rc_track_right_banked_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_flat_to_left_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC0A4 */
static void twister_rc_track_left_quarter_turn_1_90_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17998, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17999, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18007, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18008, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18001, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18009, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
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

/** rct2: 0x008AC0B4 */
static void twister_rc_track_right_quarter_turn_1_90_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
	case 0:
		switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18002, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18010, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18011, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18004, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18012, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18005, 0, 0, 2, 20, 63, height, 4, 6, height + 8);
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

/** rct2: 0x008AC0C4 */
static void twister_rc_track_left_quarter_turn_1_90_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_right_quarter_turn_1_90_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008AC0D4 */
static void twister_rc_track_right_quarter_turn_1_90_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_left_quarter_turn_1_90_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/* The following track elements used to be specific to the Vertical Roller Coaster */
static void twister_rc_track_flat_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_60_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_flat_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_60_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

static void twister_rc_track_brake_for_drop(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_brake_for_drop(rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_TUBES);
}

/* The following track elements used to be specific to the Steel Twister */
/** rct2: 0x008ABCD4 */
static void twister_rc_track_half_loop_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    switch (trackSequence) {
        case 0:
            switch (direction) {
                case 0:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17626, 0, 6, 32, 20, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17634, 0, 6, 32, 20, 11, height);
                    break;
                case 2:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17633, 0, 6, 32, 20, 9, height);
                    break;
                case 3:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17641, 0, 6, 32, 20, 7, height);
                    break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3) {
                paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
            }
            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(height + 56, 0x20);
            break;
        case 1:
            switch (direction) {
                case 0:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17627, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17635, 0, 14, 3, 20, 63, height, 28, 6, height);
                    metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 15, height, gTrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17632, 0, 6, 3, 20, 63, height, 28, 6, height);
                    metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17640, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(height + 72, 0x20);
            break;
        case 2:
            switch (direction) {
                case 0:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17628, 16, 0, 2, 16, 119, height);
                    break;
                case 1:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17636, 12, 0, 3, 16, 119, height, 12, 0, height);
                    break;
                case 2:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17631, 10, 16, 4, 12, 119, height);
                    break;
                case 3:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17639, 16, 16, 2, 16, 119, height, 15, 6, height);
                    break;
            }
            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(height + 168, 0x20);
            break;
        case 3:
            switch (direction) {
                case 0:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17629, 0, 0, 32, 16, 3, height + 32);
                    break;
                case 1:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17637, 0, 0, 32, 16, 3, height + 32);
                    break;
                case 2:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17630, 0, 16, 32, 12, 3, height + 32);
                    break;
                case 3:
                    sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17638, 0, 16, 32, 12, 3, height + 32);
                    break;
            }
            if (direction == 0 || direction == 3) {
                paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
            }
            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008ABCE4 */
static void twister_rc_track_half_loop_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    twister_rc_track_half_loop_up(rideIndex, 3 - trackSequence, direction, height, mapElement);
}

/** rct2: 0x008ABD34 */
static void twister_rc_track_left_corkscrew_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    switch (trackSequence) {
        case 0:
            switch (direction) {
                case 0:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17690, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
                    break;
                case 1:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17693, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
                    break;
                case 2:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17696, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
                    break;
                case 3:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17699, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
                    break;
            }

            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3) {
                paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
            }
            paint_util_set_general_support_height(height + 48, 0x20);
            break;
        case 1:
            switch (direction) {
                case 0:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17691, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
                    break;
                case 1:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17694, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
                    break;
                case 2:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17697, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
                    break;
                case 3:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17700, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
                    break;
            }
            paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(height + 72, 0x20);
            break;
        case 2:
            switch (direction) {
                case 0:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17692, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
                    break;
                case 1:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17695, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
                    break;
                case 2:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17698, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
                    break;
                case 3:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17701, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
                    break;
            }

            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height + 35, gTrackColours[SCHEME_SUPPORTS]);

            switch (direction) {
                case 2:
                    paint_util_push_tunnel_right(height + 8, TUNNEL_6);
                    break;
                case 3:
                    paint_util_push_tunnel_left(height + 8, TUNNEL_6);
                    break;
            }
            paint_util_set_general_support_height(height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008ABD44 */
static void twister_rc_track_right_corkscrew_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    switch (trackSequence) {
        case 0:
            switch (direction) {
                case 0:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17702, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
                    break;
                case 1:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17705, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
                    break;
                case 2:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17708, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
                    break;
                case 3:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17711, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
                    break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3) {
                paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
            }
            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(height + 48, 0x20);
            break;
        case 1:
            switch (direction) {
                case 0:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17703, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
                    break;
                case 1:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17706, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
                    break;
                case 2:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17709, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
                    break;
                case 3:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17712, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
                    break;
            }
            paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(height + 72, 0x20);
            break;
        case 2:
            switch (direction) {
                case 0:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17704, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
                    break;
                case 1:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17707, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
                    break;
                case 2:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17710, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
                    break;
                case 3:
                    sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17713, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
                    break;
            }

            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height + 35, gTrackColours[SCHEME_SUPPORTS]);

            switch (direction) {
                case 0:
                    paint_util_push_tunnel_right(height + 8, TUNNEL_6);
                    break;
                case 1:
                    paint_util_push_tunnel_left(height + 8, TUNNEL_6);
                    break;
            }
            paint_util_set_general_support_height(height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008ABD54 */
static void twister_rc_track_left_corkscrew_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    twister_rc_track_right_corkscrew_up(rideIndex, 2 - trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008ABD64 */
static void twister_rc_track_right_corkscrew_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    twister_rc_track_left_corkscrew_up(rideIndex, 2 - trackSequence, (direction - 1) & 3, height, mapElement);
}

/* The following track elements used to be specific to the Twister RC */
/** rct2: 0x008AC0F4 */
static void twister_rc_track_flat_to_60_deg_up_long_base(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
		case 0:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18030, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18034, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18038, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18042, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
				metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
			}
			if (direction == 0 || direction == 3) {
				paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 48, 0x20);
			break;
		case 1:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18031, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18035, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18039, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18043, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
				metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 48, 0x20);
			break;
		case 2:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18032, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18036, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18040, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18044, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
				metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 64, 0x20);
			break;
		case 3:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18033, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18037, 0, 0, 32, 1, 98, height, 0, 27, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18041, 0, 0, 32, 1, 98, height, 0, 27, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18045, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
				metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 18, height, gTrackColours[SCHEME_SUPPORTS]);
			}
			switch (direction) {
				case 1:
					paint_util_push_tunnel_right(height + 24, TUNNEL_8);
					break;
				case 2:
					paint_util_push_tunnel_left(height + 24, TUNNEL_8);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 80, 0x20);
			break;
	}
}

/** rct2: 0x008AC104 */
static void twister_rc_track_60_deg_up_to_flat_long_base(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
		case 0:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18046, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18050, 0, 0, 32, 1, 98, height, 0, 27, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18054, 0, 0, 32, 1, 98, height, 0, 27, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18058, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
				metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
			}
			if (direction == 0 || direction == 3) {
				paint_util_push_tunnel_rotated(direction, height, TUNNEL_7);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 80, 0x20);
			break;
		case 1:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18047, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18051, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18055, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18059, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
				metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 80, 0x20);
			break;
		case 2:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18048, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18052, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18056, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18060, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
				metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 13, height, gTrackColours[SCHEME_SUPPORTS]);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 56, 0x20);
			break;
		case 3:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18049, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18053, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18057, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18061, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
				metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 5, height, gTrackColours[SCHEME_SUPPORTS]);
			}
			switch (direction) {
				case 1:
					paint_util_push_tunnel_right(height + 8, TUNNEL_6);
					break;
				case 2:
					paint_util_push_tunnel_left(height + 8, TUNNEL_6);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 40, 0x20);
			break;
	}
}

/** rct2: 0x008AC114 */
static void twister_rc_track_flat_to_60_deg_down_long_base(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_flat_to_60_deg_up_long_base(rideIndex, 3 - trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC124 */
static void twister_rc_track_60_deg_up_to_flat_long_base122(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_60_deg_up_to_flat_long_base(rideIndex, 3 - trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ABCF4 */
static void twister_rc_track_left_barrel_roll_up_to_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
		case 0:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17642, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17666, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 2, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17645, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17669, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 2, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17648, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17672, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 2, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17651, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17675, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 2, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
			}
			if (direction == 0 || direction == 3) {
				paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 32, 0x20);
			break;
		case 1:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17643, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17667, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17646, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17670, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17649, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17673, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17652, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17676, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 48, 0x20);
			break;
		case 2:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17644, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17668, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17647, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17671, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17650, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17674, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17653, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17677, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
					break;
			}
			switch (direction) {
				case 1:
					paint_util_push_tunnel_right(height, TUNNEL_9);
					break;
				case 2:
					paint_util_push_tunnel_left(height, TUNNEL_9);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 48, 0x20);
			break;
	}
}

/** rct2: 0x008ABD04 */
static void twister_rc_track_right_barrel_roll_up_to_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
		case 0:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17654, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17678, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 2, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17657, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17681, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 2, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17660, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17684, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 2, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17663, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17687, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 2, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
			}
			if (direction == 0 || direction == 3) {
				paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 32, 0x20);
			break;
		case 1:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17655, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17679, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17658, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17682, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17661, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17685, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17664, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17688, 0, 0, 32, 20, 0, height, 0, 6, height + 28);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 48, 0x20);
			break;
		case 2:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17656, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17680, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17659, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17683, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17662, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17686, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17665, 0, 0, 32, 20, 3, height, 0, 6, height);
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17689, 0, 0, 32, 20, 0, height, 0, 6, height + 44);
					break;
			}
			switch (direction) {
				case 1:
					paint_util_push_tunnel_right(height, TUNNEL_9);
					break;
				case 2:
					paint_util_push_tunnel_left(height, TUNNEL_9);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 48, 0x20);
			break;
	}
}

/** rct2: 0x008ABD14 */
static void twister_rc_track_left_barrel_roll_down_to_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_left_barrel_roll_up_to_down(rideIndex, 2 - trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ABD24 */
static void twister_rc_track_right_barrel_roll_down_to_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_right_barrel_roll_up_to_down(rideIndex, 2 - trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ABDB4 */
static void twister_rc_track_powered_lift(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (direction) {
		case 0:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17476, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 1:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17477, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 2:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17478, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
		case 3:
			sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17479, 0, 0, 32, 20, 3, height, 0, 6, height);
			break;
	}
	metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
	if (direction == 0 || direction == 3) {
		paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
	} else {
		paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ABDC4 */
static void twister_rc_track_left_large_half_loop_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
		case 0:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17732, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17739, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17746, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17753, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			if (direction == 0 || direction == 3) {
				paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 56, 0x20);
			break;
		case 1:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17733, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17740, 0, 0, 32, 20, 9, height, 0, 6, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17747, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17754, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 72, 0x20);
			break;
		case 2:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17734, 0, 0, 32, 16, 3, height, 0, 0, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17741, 0, 0, 32, 16, 0, height, 0, 0, height + 70);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17748, 0, 0, 32, 16, 0, height, 0, 16, height + 70);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17755, 0, 0, 32, 16, 3, height, 0, 16, height);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 88, 0x20);
			break;
		case 3:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17735, 0, 0, 32, 16, 3, height, 0, 0, height);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 20, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17742, 0, 0, 32, 16, 0, height, 0, 0, height + 200);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 22, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17749, 0, 0, 32, 16, 0, height, 0, 16, height + 200);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 20, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17756, 0, 0, 32, 16, 3, height, 0, 16, height);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 20, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 224, 0x20);
			break;
		case 4:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17736, 0, 0, 16, 16, 3, height, 16, 16, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17743, 0, 0, 16, 16, 0, height, 0, 16, height + 110);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17750, 0, 0, 16, 16, 0, height, 0, 0, height + 100);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17757, 0, 0, 16, 16, 3, height, 16, 0, height);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 128, 0x20);
			break;
		case 5:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17737, 0, 0, 32, 16, 3, height, 0, 16, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17744, 0, 0, 32, 16, 0, height, 0, 16, height + 200);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17751, 0, 0, 32, 16, 0, height, 0, 0, height + 200);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17758, 0, 0, 32, 16, 3, height, 0, 0, height);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 224, 0x20);
			break;
		case 6:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17738, 0, 0, 32, 16, 0, height, 0, 16, height + 32);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17745, 0, 0, 32, 16, 0, height, 0, 16, height + 32);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17752, 0, 0, 32, 16, 0, height, 0, 0, height + 32);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17759, 0, 0, 32, 16, 0, height, 0, 0, height + 32);
					break;
			}
			if (direction == 0 || direction == 3) {
				paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 40, 0x20);
			break;
	}
}

/** rct2: 0x008ABDD4 */
static void twister_rc_track_right_large_half_loop_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
		case 0:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17781, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17774, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17767, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17760, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
			if (direction == 0 || direction == 3) {
				paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 56, 0x20);
			break;
		case 1:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17782, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17775, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17768, 0, 0, 32, 20, 9, height, 0, 6, height);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17761, 0, 0, 32, 20, 3, height, 0, 6, height);
					break;
			}
			metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 72, 0x20);
			break;
		case 2:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17783, 0, 0, 32, 16, 3, height, 0, 16, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17776, 0, 0, 32, 16, 0, height, 0, 16, height + 70);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17769, 0, 0, 32, 16, 0, height, 0, 0, height + 70);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17762, 0, 0, 32, 16, 3, height, 0, 0, height);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 88, 0x20);
			break;
		case 3:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17784, 0, 0, 32, 16, 3, height, 0, 16, height);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 20, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17777, 0, 0, 32, 16, 0, height, 0, 16, height + 200);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 20, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17770, 0, 0, 32, 16, 0, height, 0, 0, height + 200);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 22, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17763, 0, 0, 32, 16, 3, height, 0, 0, height);
					metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 20, height, gTrackColours[SCHEME_SUPPORTS]);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 224, 0x20);
			break;
		case 4:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17785, 0, 0, 16, 16, 3, height, 16, 0, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17778, 0, 0, 16, 16, 0, height, 0, 0, height + 100);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17771, 0, 0, 16, 16, 0, height, 0, 16, height + 110);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17764, 0, 0, 16, 16, 3, height, 16, 16, height);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 128, 0x20);
			break;
		case 5:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17786, 0, 0, 32, 16, 3, height, 0, 0, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17779, 0, 0, 32, 16, 0, height, 0, 0, height + 200);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17772, 0, 0, 32, 16, 0, height, 0, 16, height + 200);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17765, 0, 0, 32, 16, 3, height, 0, 16, height);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 224, 0x20);
			break;
		case 6:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17787, 0, 0, 32, 16, 0, height, 0, 0, height + 32);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17780, 0, 0, 32, 16, 0, height, 0, 0, height + 32);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17773, 0, 0, 32, 16, 0, height, 0, 16, height + 32);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 17766, 0, 0, 32, 16, 0, height, 0, 16, height + 32);
					break;
			}
			if (direction == 0 || direction == 3) {
				paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 40, 0x20);
			break;
	}
}

/** rct2: 0x008ABDE4 */
static void twister_rc_track_right_large_half_loop_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_right_large_half_loop_up(rideIndex, 6 - trackSequence, direction, height, mapElement);
}

/** rct2: 0x008ABDF4 */
static void twister_rc_track_left_large_half_loop_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_left_large_half_loop_up(rideIndex, 6 - trackSequence, direction, height, mapElement);
}

/** rct2: 0x008AC134 */
static void twister_rc_track_90_deg_to_inverted_flat_quarter_loop_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	switch (trackSequence) {
		case 0:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18062, 0, 0, 2, 20, 31, height, 4, 6, height + 8);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18065, 0, 0, 2, 20, 31, height, 24, 6, height + 8);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18068, 0, 0, 2, 20, 31, height, 24, 6, height + 8);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18071, 0, 0, 2, 20, 31, height, 4, 6, height + 8);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 88, 0x20);
			break;
		case 1:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18063, 0, 0, 2, 20, 31, height, -8, 6, height);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18066, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18069, 0, 0, 2, 20, 63, height, 24, 6, height + 8);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18072, 0, 0, 2, 20, 31, height, -8, 6, height);
					break;
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 64, 0x20);
			break;
		case 2:
			switch (direction) {
				case 0:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18064, 0, 0, 32, 20, 3, height, 0, 6, height + 24);
					break;
				case 1:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18067, 0, 0, 2, 20, 31, height, 24, 6, height + 8);
					break;
				case 2:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18070, 0, 0, 2, 20, 31, height, 24, 6, height + 8);
					break;
				case 3:
					sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 18073, 0, 0, 32, 20, 3, height, 0, 6, height + 24);
					break;
			}
			if (direction == 0 || direction == 3) {
				paint_util_push_tunnel_rotated(direction, height + 16, TUNNEL_6);
			}
			paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
			paint_util_set_general_support_height(height + 48, 0x20);
			break;
	}
}

/** rct2: 0x008AC144 */
static void twister_rc_track_inverted_flat_to_90_deg_quarter_loop_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	twister_rc_track_90_deg_to_inverted_flat_quarter_loop_up(rideIndex, 2 - trackSequence, direction, height, mapElement);
}


TRACK_PAINT_FUNCTION get_track_paint_function_twister_rc(int trackType, int direction)
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
	/* The following track elements used to be specific to the Twister RC */
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
	}
	return NULL;
}
