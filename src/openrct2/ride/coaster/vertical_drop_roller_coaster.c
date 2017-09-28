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
#include "../track_data.h"
#include "bolliger_mabillard_track.h"

/** rct2: 0x008AA00C */
static void vertical_drop_rc_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                        sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                           sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_station(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA01C */
static void vertical_drop_rc_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                             sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA02C */
static void vertical_drop_rc_track_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                             sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA03C */
static void vertical_drop_rc_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                     uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                               METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA04C */
static void vertical_drop_rc_track_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA05C */
static void vertical_drop_rc_track_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA06C */
static void vertical_drop_rc_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                     uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                               METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA07C */
static void vertical_drop_rc_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                               sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                         METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA08C */
static void vertical_drop_rc_track_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                               sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                         METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA09C */
static void vertical_drop_rc_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                       uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                 METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0AC */
static void vertical_drop_rc_track_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                              uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                        METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0BC */
static void vertical_drop_rc_track_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                              uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_down_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                        METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0CC */
static void vertical_drop_rc_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                       uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                 METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0DC */
static void vertical_drop_rc_track_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                       uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_5(session, rideIndex, trackSequence, direction, height, mapElement,
                                                 METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0EC */
static void vertical_drop_rc_track_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                        uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_5(session, rideIndex, trackSequence, direction, height, mapElement,
                                                  METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA0FC */
static void vertical_drop_rc_track_flat_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                     uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                               METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA10C */
static void vertical_drop_rc_track_flat_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                      uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA11C */
static void vertical_drop_rc_track_left_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                     uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                               METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA12C */
static void vertical_drop_rc_track_right_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                      uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA13C */
static void vertical_drop_rc_track_banked_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                              uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_banked_left_quarter_turn_5(session, rideIndex, trackSequence, direction, height, mapElement,
                                                        METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA14C */
static void vertical_drop_rc_track_banked_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                               uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_banked_right_quarter_turn_5(session, rideIndex, trackSequence, direction, height, mapElement,
                                                         METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA15C */
static void vertical_drop_rc_track_left_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA16C */
static void vertical_drop_rc_track_right_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                           uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                     METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA17C */
static void vertical_drop_rc_track_25_deg_up_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA18C */
static void vertical_drop_rc_track_25_deg_up_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                           uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                     METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA19C */
static void vertical_drop_rc_track_left_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                            uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                      METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1AC */
static void vertical_drop_rc_track_right_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                             uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                       METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1BC */
static void vertical_drop_rc_track_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                            uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                      METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1CC */
static void vertical_drop_rc_track_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                             uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                       METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1DC */
static void vertical_drop_rc_track_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                             sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1EC */
static void vertical_drop_rc_track_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                              sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA1FC */
static void vertical_drop_rc_track_left_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                 uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                           METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA20C */
static void vertical_drop_rc_track_right_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA21C */
static void vertical_drop_rc_track_left_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_5_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA22C */
static void vertical_drop_rc_track_right_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                    uint8 trackSequence, uint8 direction, sint32 height,
                                                                    rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_5_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                              METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA23C */
static void vertical_drop_rc_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                               sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_s_bend_left(session, rideIndex, trackSequence, direction, height, mapElement,
                                         METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA24C */
static void vertical_drop_rc_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                                sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_s_bend_right(session, rideIndex, trackSequence, direction, height, mapElement,
                                          METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA49C */
static void vertical_drop_rc_track_left_vertical_loop(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                      uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_vertical_loop(session, rideIndex, trackSequence, direction, height, mapElement,
                                                METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4AC */
static void vertical_drop_rc_track_right_vertical_loop(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                       uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_vertical_loop(session, rideIndex, trackSequence, direction, height, mapElement,
                                                 METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA28C */
static void vertical_drop_rc_track_left_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                       uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3(session, rideIndex, trackSequence, direction, height, mapElement,
                                                 METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA29C */
static void vertical_drop_rc_track_right_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                        uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3(session, rideIndex, trackSequence, direction, height, mapElement,
                                                  METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2AC */
static void vertical_drop_rc_track_left_quarter_turn_3_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                            uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                      METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2BC */
static void vertical_drop_rc_track_right_quarter_turn_3_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                             uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                       METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2CC */
static void vertical_drop_rc_track_left_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                 uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                           METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2DC */
static void vertical_drop_rc_track_right_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2EC */
static void vertical_drop_rc_track_left_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA2FC */
static void vertical_drop_rc_track_right_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                    uint8 trackSequence, uint8 direction, sint32 height,
                                                                    rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                              METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA30C */
static void vertical_drop_rc_track_left_half_banked_helix_up_small(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_half_banked_helix_up_small(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA31C */
static void vertical_drop_rc_track_right_half_banked_helix_up_small(paint_session * session, uint8 rideIndex,
                                                                    uint8 trackSequence, uint8 direction, sint32 height,
                                                                    rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_half_banked_helix_up_small(session, rideIndex, trackSequence, direction, height, mapElement,
                                                              METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA32C */
static void vertical_drop_rc_track_left_half_banked_helix_down_small(paint_session * session, uint8 rideIndex,
                                                                     uint8 trackSequence, uint8 direction, sint32 height,
                                                                     rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_half_banked_helix_down_small(session, rideIndex, trackSequence, direction, height, mapElement,
                                                               METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA33C */
static void vertical_drop_rc_track_right_half_banked_helix_down_small(paint_session * session, uint8 rideIndex,
                                                                      uint8 trackSequence, uint8 direction, sint32 height,
                                                                      rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_half_banked_helix_down_small(session, rideIndex, trackSequence, direction, height,
                                                                mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA34C */
static void vertical_drop_rc_track_left_half_banked_helix_up_large(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_half_banked_helix_up_large(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA35C */
static void vertical_drop_rc_track_right_half_banked_helix_up_large(paint_session * session, uint8 rideIndex,
                                                                    uint8 trackSequence, uint8 direction, sint32 height,
                                                                    rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_half_banked_helix_up_large(session, rideIndex, trackSequence, direction, height, mapElement,
                                                              METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA36C */
static void vertical_drop_rc_track_left_half_banked_helix_down_large(paint_session * session, uint8 rideIndex,
                                                                     uint8 trackSequence, uint8 direction, sint32 height,
                                                                     rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_half_banked_helix_down_large(session, rideIndex, trackSequence, direction, height, mapElement,
                                                               METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA37C */
static void vertical_drop_rc_track_right_half_banked_helix_down_large(paint_session * session, uint8 rideIndex,
                                                                      uint8 trackSequence, uint8 direction, sint32 height,
                                                                      rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_half_banked_helix_down_large(session, rideIndex, trackSequence, direction, height,
                                                                mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA3AC */
static void vertical_drop_rc_track_left_quarter_turn_1_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                 uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                           METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA38C */
static void vertical_drop_rc_track_right_quarter_turn_1_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA39C */
static void vertical_drop_rc_track_left_quarter_turn_1_60_deg_down(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA3BC */
static void vertical_drop_rc_track_right_quarter_turn_1_60_deg_down(paint_session * session, uint8 rideIndex,
                                                                    uint8 trackSequence, uint8 direction, sint32 height,
                                                                    rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                              METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA40C */
static void vertical_drop_rc_track_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                          sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_brakes(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7EC */
static void vertical_drop_rc_track_25_deg_up_left_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                         uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_left_banked(session, rideIndex, trackSequence, direction, height, mapElement,
                                                   METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7FC */
static void vertical_drop_rc_track_25_deg_up_right_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_right_banked(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA41C */
static void vertical_drop_rc_track_on_ride_photo(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                                 sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_on_ride_photo(session, rideIndex, trackSequence, direction, height, mapElement,
                                           METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA80C */
static void vertical_drop_rc_track_25_deg_down_left_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                           uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_left_banked(session, rideIndex, trackSequence, direction, height, mapElement,
                                                     METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA81C */
static void vertical_drop_rc_track_25_deg_down_right_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                            uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_right_banked(session, rideIndex, trackSequence, direction, height, mapElement,
                                                      METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA42C */
static void vertical_drop_rc_track_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                             sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_90_deg_up(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA43C */
static void vertical_drop_rc_track_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                               sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_90_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                         METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA44C */
static void vertical_drop_rc_track_60_deg_up_to_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_90_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA45C */
static void vertical_drop_rc_track_90_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                              uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_90_deg_down_to_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                        METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA46C */
static void vertical_drop_rc_track_90_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_90_deg_up_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA47C */
static void vertical_drop_rc_track_60_deg_down_to_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                              uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_down_to_90_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                        METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4CC */
static void vertical_drop_rc_track_left_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                       uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_eighth_to_diag(session, rideIndex, trackSequence, direction, height, mapElement,
                                                 METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4DC */
static void vertical_drop_rc_track_right_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                        uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_eighth_to_diag(session, rideIndex, trackSequence, direction, height, mapElement,
                                                  METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4EC */
static void vertical_drop_rc_track_left_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                             uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_eighth_to_orthogonal(session, rideIndex, trackSequence, direction, height, mapElement,
                                                       METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4FC */
static void vertical_drop_rc_track_right_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                              uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_eighth_to_orthogonal(session, rideIndex, trackSequence, direction, height, mapElement,
                                                        METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA50C */
static void vertical_drop_rc_track_left_eighth_bank_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                            uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_eighth_bank_to_diag(session, rideIndex, trackSequence, direction, height, mapElement,
                                                      METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA51C */
static void vertical_drop_rc_track_right_eighth_bank_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                             uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_eighth_bank_to_diag(session, rideIndex, trackSequence, direction, height, mapElement,
                                                       METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA52C */
static void vertical_drop_rc_track_left_eighth_bank_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_eighth_bank_to_orthogonal(session, rideIndex, trackSequence, direction, height, mapElement,
                                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA53C */
static void vertical_drop_rc_track_right_eighth_bank_to_orthogonal(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_eighth_bank_to_orthogonal(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA4BC */
static void vertical_drop_rc_track_diag_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                             sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA56C */
static void vertical_drop_rc_track_diag_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5CC */
static void vertical_drop_rc_track_diag_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA54C */
static void vertical_drop_rc_track_diag_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5AC */
static void vertical_drop_rc_track_diag_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                               uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                         METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5BC */
static void vertical_drop_rc_track_diag_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                               uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                         METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA55C */
static void vertical_drop_rc_track_diag_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA59C */
static void vertical_drop_rc_track_diag_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                    uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                              METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5FC */
static void vertical_drop_rc_track_diag_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                    uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                              METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA57C */
static void vertical_drop_rc_track_diag_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                            uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                      METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5DC */
static void vertical_drop_rc_track_diag_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA5EC */
static void vertical_drop_rc_track_diag_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_down_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA58C */
static void vertical_drop_rc_track_diag_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                            uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                      METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA62C */
static void vertical_drop_rc_track_diag_flat_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA63C */
static void vertical_drop_rc_track_diag_flat_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                           uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                     METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA64C */
static void vertical_drop_rc_track_diag_left_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_left_bank_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA65C */
static void vertical_drop_rc_track_diag_right_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                           uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_right_bank_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                     METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA68C */
static void vertical_drop_rc_track_diag_left_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                               uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_left_bank_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                         METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA69C */
static void vertical_drop_rc_track_diag_right_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_right_bank_to_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                          METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA66C */
static void vertical_drop_rc_track_diag_25_deg_up_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                               uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                         METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA67C */
static void vertical_drop_rc_track_diag_25_deg_up_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_up_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                          METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6AC */
static void vertical_drop_rc_track_diag_left_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                 uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_left_bank_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                           METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6BC */
static void vertical_drop_rc_track_diag_right_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_right_bank_to_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6CC */
static void vertical_drop_rc_track_diag_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                 uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_left_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                           METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6DC */
static void vertical_drop_rc_track_diag_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_25_deg_down_to_right_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA60C */
static void vertical_drop_rc_track_diag_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_left_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA61C */
static void vertical_drop_rc_track_diag_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                   uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_right_bank(session, rideIndex, trackSequence, direction, height, mapElement,
                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA72C */
static void vertical_drop_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                              uint8 trackSequence, uint8 direction,
                                                                              sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_bank_to_left_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                        mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA73C */
static void vertical_drop_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                                uint8 trackSequence, uint8 direction,
                                                                                sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_bank_to_right_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                          mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA74C */
static void vertical_drop_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex,
                                                                                uint8 trackSequence, uint8 direction,
                                                                                sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_3_25_deg_down_to_left_bank(session, rideIndex, trackSequence, direction, height,
                                                                          mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA75C */
static void vertical_drop_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex,
                                                                                  uint8 trackSequence, uint8 direction,
                                                                                  sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_3_25_deg_down_to_right_bank(session, rideIndex, trackSequence, direction,
                                                                            height, mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA9EC */
static void vertical_drop_rc_track_block_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                                sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_block_brakes(session, rideIndex, trackSequence, direction, height, mapElement,
                                          METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA96C */
static void vertical_drop_rc_track_left_banked_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                        uint8 trackSequence, uint8 direction, sint32 height,
                                                                        rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                  mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA97C */
static void vertical_drop_rc_track_right_banked_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                         uint8 trackSequence, uint8 direction, sint32 height,
                                                                         rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                   mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA98C */
static void vertical_drop_rc_track_left_banked_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                          uint8 trackSequence, uint8 direction, sint32 height,
                                                                          rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                    mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA99C */
static void vertical_drop_rc_track_right_banked_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                           uint8 trackSequence, uint8 direction, sint32 height,
                                                                           rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                     mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8AC */
static void vertical_drop_rc_track_left_banked_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                        uint8 trackSequence, uint8 direction, sint32 height,
                                                                        rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                  mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8BC */
static void vertical_drop_rc_track_right_banked_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                         uint8 trackSequence, uint8 direction, sint32 height,
                                                                         rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                   mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8CC */
static void vertical_drop_rc_track_left_banked_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                          uint8 trackSequence, uint8 direction, sint32 height,
                                                                          rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                    mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8DC */
static void vertical_drop_rc_track_right_banked_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                           uint8 trackSequence, uint8 direction, sint32 height,
                                                                           rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_quarter_turn_5_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                     mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA82C */
static void vertical_drop_rc_track_25_deg_up_to_left_banked_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                      uint8 trackSequence, uint8 direction, sint32 height,
                                                                      rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_left_banked_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA83C */
static void vertical_drop_rc_track_25_deg_up_to_right_banked_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                       uint8 trackSequence, uint8 direction, sint32 height,
                                                                       rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_up_to_right_banked_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                 mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA84C */
static void vertical_drop_rc_track_left_banked_25_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                      uint8 trackSequence, uint8 direction, sint32 height,
                                                                      rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA85C */
static void vertical_drop_rc_track_right_banked_25_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                       uint8 trackSequence, uint8 direction, sint32 height,
                                                                       rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                 mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA86C */
static void vertical_drop_rc_track_25_deg_down_to_left_banked_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                          uint8 trackSequence, uint8 direction, sint32 height,
                                                                          rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_left_banked_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                    mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA87C */
static void vertical_drop_rc_track_25_deg_down_to_right_banked_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                           uint8 trackSequence, uint8 direction, sint32 height,
                                                                           rct_map_element * mapElement)
{
    bolliger_mabillard_track_25_deg_down_to_right_banked_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                     mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA88C */
static void vertical_drop_rc_track_left_banked_25_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                          uint8 trackSequence, uint8 direction, sint32 height,
                                                                          rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                    mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA89C */
static void vertical_drop_rc_track_right_banked_25_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                           uint8 trackSequence, uint8 direction, sint32 height,
                                                                           rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                     mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8EC */
static void vertical_drop_rc_track_left_banked_flat_to_left_banked_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                             uint8 trackSequence, uint8 direction,
                                                                             sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_flat_to_left_banked_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                       mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA8FC */
static void vertical_drop_rc_track_right_banked_flat_to_right_banked_25_deg_up(paint_session * session, uint8 rideIndex,
                                                                               uint8 trackSequence, uint8 direction,
                                                                               sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_flat_to_right_banked_25_deg_up(session, rideIndex, trackSequence, direction, height,
                                                                         mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA92C */
static void vertical_drop_rc_track_left_banked_25_deg_up_to_left_banked_flat(paint_session * session, uint8 rideIndex,
                                                                             uint8 trackSequence, uint8 direction,
                                                                             sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_left_banked_flat(session, rideIndex, trackSequence, direction, height,
                                                                       mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA93C */
static void vertical_drop_rc_track_right_banked_25_deg_up_to_right_banked_flat(paint_session * session, uint8 rideIndex,
                                                                               uint8 trackSequence, uint8 direction,
                                                                               sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_right_banked_flat(session, rideIndex, trackSequence, direction, height,
                                                                         mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA94C */
static void vertical_drop_rc_track_left_banked_flat_to_left_banked_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                               uint8 trackSequence, uint8 direction,
                                                                               sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_flat_to_left_banked_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                         mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA95C */
static void vertical_drop_rc_track_right_banked_flat_to_right_banked_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                                 uint8 trackSequence, uint8 direction,
                                                                                 sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_flat_to_right_banked_25_deg_down(session, rideIndex, trackSequence, direction, height,
                                                                           mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA90C */
static void vertical_drop_rc_track_left_banked_25_deg_down_to_left_banked_flat(paint_session * session, uint8 rideIndex,
                                                                               uint8 trackSequence, uint8 direction,
                                                                               sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_left_banked_flat(session, rideIndex, trackSequence, direction, height,
                                                                         mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA91C */
static void vertical_drop_rc_track_right_banked_25_deg_down_to_right_banked_flat(paint_session * session, uint8 rideIndex,
                                                                                 uint8 trackSequence, uint8 direction,
                                                                                 sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_right_banked_flat(session, rideIndex, trackSequence, direction, height,
                                                                           mapElement, METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA76C */
static void vertical_drop_rc_track_flat_to_left_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                 uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_left_banked_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                           METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA77C */
static void vertical_drop_rc_track_flat_to_right_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_right_banked_25_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA78C */
static void vertical_drop_rc_track_left_banked_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                 uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                           METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA79C */
static void vertical_drop_rc_track_right_banked_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7AC */
static void vertical_drop_rc_track_flat_to_left_banked_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_left_banked_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7BC */
static void vertical_drop_rc_track_flat_to_right_banked_25_deg_down(paint_session * session, uint8 rideIndex,
                                                                    uint8 trackSequence, uint8 direction, sint32 height,
                                                                    rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_right_banked_25_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                              METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7CC */
static void vertical_drop_rc_track_left_banked_25_deg_down_to_flat(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_banked_25_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA7DC */
static void vertical_drop_rc_track_right_banked_25_deg_down_to_flat(paint_session * session, uint8 rideIndex,
                                                                    uint8 trackSequence, uint8 direction, sint32 height,
                                                                    rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_banked_25_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                              METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA9AC */
static void vertical_drop_rc_track_left_quarter_turn_1_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                 uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_90_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                           METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA9BC */
static void vertical_drop_rc_track_right_quarter_turn_1_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_90_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA9CC */
static void vertical_drop_rc_track_left_quarter_turn_1_90_deg_down(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_quarter_turn_1_90_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                             METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA9DC */
static void vertical_drop_rc_track_right_quarter_turn_1_90_deg_down(paint_session * session, uint8 rideIndex,
                                                                    uint8 trackSequence, uint8 direction, sint32 height,
                                                                    rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_quarter_turn_1_90_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                              METAL_SUPPORTS_BOXED);
}

/* The following elements used to be specific to the vertical_drop RC */
/** rct2: 0x008AA3CC */
static void vertical_drop_rc_track_flat_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                     uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                               METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA3DC */
static void vertical_drop_rc_track_60_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                     uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                               METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA3EC */
static void vertical_drop_rc_track_flat_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                       uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                 METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA3FC */
static void vertical_drop_rc_track_60_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                       uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                 METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA48C */
static void vertical_drop_rc_track_brake_for_drop(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_brake_for_drop(session, rideIndex, trackSequence, direction, height, mapElement,
                                            METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6EC */
static void vertical_drop_rc_track_diag_flat_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_60_deg_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA6FC */
static void vertical_drop_rc_track_diag_60_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                          uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_up_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                    METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA70C */
static void vertical_drop_rc_track_diag_flat_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                            uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_flat_to_60_deg_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                      METAL_SUPPORTS_BOXED);
}

/** rct2: 0x008AA71C */
static void vertical_drop_rc_track_diag_60_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                            uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_diag_60_deg_down_to_flat(session, rideIndex, trackSequence, direction, height, mapElement,
                                                      METAL_SUPPORTS_BOXED);
}

/* The following elements used to be specific to the steel twister RC */
static void vertical_drop_rc_track_half_loop_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                                sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_half_loop_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                          METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_half_loop_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_half_loop_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                            METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_corkscrew_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                     uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_corkscrew_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                               METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_corkscrew_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                      uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_corkscrew_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_corkscrew_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                       uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_corkscrew_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                 METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_corkscrew_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                        uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_corkscrew_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                  METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_flat_to_60_deg_up_long_base(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                               uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_60_deg_up_long_base(session, rideIndex, trackSequence, direction, height, mapElement,
                                                         METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_60_deg_up_to_flat_long_base(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                               uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat_long_base(session, rideIndex, trackSequence, direction, height, mapElement,
                                                         METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_flat_to_60_deg_down_long_base(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                 uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_flat_to_60_deg_down_long_base(session, rideIndex, trackSequence, direction, height, mapElement,
                                                           METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_60_deg_up_to_flat_long_base122(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_60_deg_up_to_flat_long_base122(session, rideIndex, trackSequence, direction, height, mapElement,
                                                            METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_barrel_roll_up_to_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                               uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_barrel_roll_up_to_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                         METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_barrel_roll_up_to_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_barrel_roll_up_to_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                          METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_barrel_roll_down_to_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                               uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_barrel_roll_down_to_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                         METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_barrel_roll_down_to_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_barrel_roll_down_to_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                          METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_powered_lift(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                                sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_powered_lift(session, rideIndex, trackSequence, direction, height, mapElement,
                                          METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_large_half_loop_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                           uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_large_half_loop_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                     METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_large_half_loop_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                            uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_large_half_loop_up(session, rideIndex, trackSequence, direction, height, mapElement,
                                                      METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_right_large_half_loop_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                              uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_right_large_half_loop_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                        METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_left_large_half_loop_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                             uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_left_large_half_loop_down(session, rideIndex, trackSequence, direction, height, mapElement,
                                                       METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_90_deg_to_inverted_flat_quarter_loop_up(paint_session * session, uint8 rideIndex,
                                                                           uint8 trackSequence, uint8 direction, sint32 height,
                                                                           rct_map_element * mapElement)
{
    bolliger_mabillard_track_90_deg_to_inverted_flat_quarter_loop_up(session, rideIndex, trackSequence, direction, height,
                                                                     mapElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_inverted_flat_to_90_deg_quarter_loop_down(paint_session * session, uint8 rideIndex,
                                                                             uint8 trackSequence, uint8 direction,
                                                                             sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_inverted_flat_to_90_deg_quarter_loop_down(session, rideIndex, trackSequence, direction, height,
                                                                       mapElement, METAL_SUPPORTS_BOXED);
}

static void vertical_drop_rc_track_booster(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                           sint32 height, rct_map_element * mapElement)
{
    bolliger_mabillard_track_booster(session, rideIndex, trackSequence, direction, height, mapElement, METAL_SUPPORTS_BOXED);
}

TRACK_PAINT_FUNCTION get_track_paint_function_vertical_drop_rc(sint32 trackType, sint32 direction)
{
    switch (trackType)
    {
    case TRACK_ELEM_FLAT:
        return vertical_drop_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return vertical_drop_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return vertical_drop_rc_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return vertical_drop_rc_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return vertical_drop_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return vertical_drop_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return vertical_drop_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return vertical_drop_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return vertical_drop_rc_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return vertical_drop_rc_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return vertical_drop_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return vertical_drop_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return vertical_drop_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return vertical_drop_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return vertical_drop_rc_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return vertical_drop_rc_track_right_quarter_turn_5;
    case TRACK_ELEM_FLAT_TO_LEFT_BANK:
        return vertical_drop_rc_track_flat_to_left_bank;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
        return vertical_drop_rc_track_flat_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_FLAT:
        return vertical_drop_rc_track_left_bank_to_flat;
    case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
        return vertical_drop_rc_track_right_bank_to_flat;
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
        return vertical_drop_rc_track_banked_left_quarter_turn_5;
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
        return vertical_drop_rc_track_banked_right_quarter_turn_5;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
        return vertical_drop_rc_track_left_bank_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
        return vertical_drop_rc_track_right_bank_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
        return vertical_drop_rc_track_25_deg_up_to_left_bank;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
        return vertical_drop_rc_track_25_deg_up_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
        return vertical_drop_rc_track_left_bank_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
        return vertical_drop_rc_track_right_bank_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
        return vertical_drop_rc_track_25_deg_down_to_left_bank;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
        return vertical_drop_rc_track_25_deg_down_to_right_bank;
    case TRACK_ELEM_LEFT_BANK:
        return vertical_drop_rc_track_left_bank;
    case TRACK_ELEM_RIGHT_BANK:
        return vertical_drop_rc_track_right_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return vertical_drop_rc_track_left_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return vertical_drop_rc_track_right_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return vertical_drop_rc_track_left_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return vertical_drop_rc_track_right_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_S_BEND_LEFT:
        return vertical_drop_rc_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return vertical_drop_rc_track_s_bend_right;
    case TRACK_ELEM_LEFT_VERTICAL_LOOP:
        return vertical_drop_rc_track_left_vertical_loop;
    case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
        return vertical_drop_rc_track_right_vertical_loop;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return vertical_drop_rc_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return vertical_drop_rc_track_right_quarter_turn_3;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
        return vertical_drop_rc_track_left_quarter_turn_3_bank;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
        return vertical_drop_rc_track_right_quarter_turn_3_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return vertical_drop_rc_track_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return vertical_drop_rc_track_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return vertical_drop_rc_track_left_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return vertical_drop_rc_track_right_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
        return vertical_drop_rc_track_left_half_banked_helix_up_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
        return vertical_drop_rc_track_right_half_banked_helix_up_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
        return vertical_drop_rc_track_left_half_banked_helix_down_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
        return vertical_drop_rc_track_right_half_banked_helix_down_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
        return vertical_drop_rc_track_left_half_banked_helix_up_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
        return vertical_drop_rc_track_right_half_banked_helix_up_large;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
        return vertical_drop_rc_track_left_half_banked_helix_down_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
        return vertical_drop_rc_track_right_half_banked_helix_down_large;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP:
        return vertical_drop_rc_track_left_quarter_turn_1_60_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP:
        return vertical_drop_rc_track_right_quarter_turn_1_60_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        return vertical_drop_rc_track_left_quarter_turn_1_60_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        return vertical_drop_rc_track_right_quarter_turn_1_60_deg_down;
    case TRACK_ELEM_BRAKES:
        return vertical_drop_rc_track_brakes;
    case TRACK_ELEM_25_DEG_UP_LEFT_BANKED:
        return vertical_drop_rc_track_25_deg_up_left_banked;
    case TRACK_ELEM_25_DEG_UP_RIGHT_BANKED:
        return vertical_drop_rc_track_25_deg_up_right_banked;
    case TRACK_ELEM_ON_RIDE_PHOTO:
        return vertical_drop_rc_track_on_ride_photo;
    case TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED:
        return vertical_drop_rc_track_25_deg_down_left_banked;
    case TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED:
        return vertical_drop_rc_track_25_deg_down_right_banked;
    case TRACK_ELEM_90_DEG_UP:
        return vertical_drop_rc_track_90_deg_up;
    case TRACK_ELEM_90_DEG_DOWN:
        return vertical_drop_rc_track_90_deg_down;
    case TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP:
        return vertical_drop_rc_track_60_deg_up_to_90_deg_up;
    case TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN:
        return vertical_drop_rc_track_90_deg_down_to_60_deg_down;
    case TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP:
        return vertical_drop_rc_track_90_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN:
        return vertical_drop_rc_track_60_deg_down_to_90_deg_down;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
        return vertical_drop_rc_track_left_eighth_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
        return vertical_drop_rc_track_right_eighth_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        return vertical_drop_rc_track_left_eighth_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        return vertical_drop_rc_track_right_eighth_to_orthogonal;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
        return vertical_drop_rc_track_left_eighth_bank_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
        return vertical_drop_rc_track_right_eighth_bank_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
        return vertical_drop_rc_track_left_eighth_bank_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
        return vertical_drop_rc_track_right_eighth_bank_to_orthogonal;
    case TRACK_ELEM_DIAG_FLAT:
        return vertical_drop_rc_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return vertical_drop_rc_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP:
        return vertical_drop_rc_track_diag_60_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return vertical_drop_rc_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
        return vertical_drop_rc_track_diag_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
        return vertical_drop_rc_track_diag_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return vertical_drop_rc_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return vertical_drop_rc_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN:
        return vertical_drop_rc_track_diag_60_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return vertical_drop_rc_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
        return vertical_drop_rc_track_diag_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
        return vertical_drop_rc_track_diag_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return vertical_drop_rc_track_diag_25_deg_down_to_flat;
    case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
        return vertical_drop_rc_track_diag_flat_to_left_bank;
    case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
        return vertical_drop_rc_track_diag_flat_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
        return vertical_drop_rc_track_diag_left_bank_to_flat;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
        return vertical_drop_rc_track_diag_right_bank_to_flat;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
        return vertical_drop_rc_track_diag_left_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
        return vertical_drop_rc_track_diag_right_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
        return vertical_drop_rc_track_diag_25_deg_up_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
        return vertical_drop_rc_track_diag_25_deg_up_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
        return vertical_drop_rc_track_diag_left_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
        return vertical_drop_rc_track_diag_right_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
        return vertical_drop_rc_track_diag_25_deg_down_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
        return vertical_drop_rc_track_diag_25_deg_down_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK:
        return vertical_drop_rc_track_diag_left_bank;
    case TRACK_ELEM_DIAG_RIGHT_BANK:
        return vertical_drop_rc_track_diag_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return vertical_drop_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return vertical_drop_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK:
        return vertical_drop_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK:
        return vertical_drop_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank;
    case TRACK_ELEM_BLOCK_BRAKES:
        return vertical_drop_rc_track_block_brakes;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
        return vertical_drop_rc_track_left_banked_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
        return vertical_drop_rc_track_right_banked_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        return vertical_drop_rc_track_left_banked_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        return vertical_drop_rc_track_right_banked_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
        return vertical_drop_rc_track_left_banked_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
        return vertical_drop_rc_track_right_banked_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        return vertical_drop_rc_track_left_banked_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        return vertical_drop_rc_track_right_banked_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP:
        return vertical_drop_rc_track_25_deg_up_to_left_banked_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP:
        return vertical_drop_rc_track_25_deg_up_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP:
        return vertical_drop_rc_track_left_banked_25_deg_up_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP:
        return vertical_drop_rc_track_right_banked_25_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN:
        return vertical_drop_rc_track_25_deg_down_to_left_banked_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN:
        return vertical_drop_rc_track_25_deg_down_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
        return vertical_drop_rc_track_left_banked_25_deg_down_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
        return vertical_drop_rc_track_right_banked_25_deg_down_to_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP:
        return vertical_drop_rc_track_left_banked_flat_to_left_banked_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
        return vertical_drop_rc_track_right_banked_flat_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT:
        return vertical_drop_rc_track_left_banked_25_deg_up_to_left_banked_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT:
        return vertical_drop_rc_track_right_banked_25_deg_up_to_right_banked_flat;
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
        return vertical_drop_rc_track_left_banked_flat_to_left_banked_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
        return vertical_drop_rc_track_right_banked_flat_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT:
        return vertical_drop_rc_track_left_banked_25_deg_down_to_left_banked_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT:
        return vertical_drop_rc_track_right_banked_25_deg_down_to_right_banked_flat;
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP:
        return vertical_drop_rc_track_flat_to_left_banked_25_deg_up;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
        return vertical_drop_rc_track_flat_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT:
        return vertical_drop_rc_track_left_banked_25_deg_up_to_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT:
        return vertical_drop_rc_track_right_banked_25_deg_up_to_flat;
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
        return vertical_drop_rc_track_flat_to_left_banked_25_deg_down;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
        return vertical_drop_rc_track_flat_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT:
        return vertical_drop_rc_track_left_banked_25_deg_down_to_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT:
        return vertical_drop_rc_track_right_banked_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP:
        return vertical_drop_rc_track_left_quarter_turn_1_90_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP:
        return vertical_drop_rc_track_right_quarter_turn_1_90_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
        return vertical_drop_rc_track_left_quarter_turn_1_90_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
        return vertical_drop_rc_track_right_quarter_turn_1_90_deg_down;
    /* These track elements used to be specific to the vertical RC */
    case TRACK_ELEM_FLAT_TO_60_DEG_UP:
        return vertical_drop_rc_track_flat_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_FLAT:
        return vertical_drop_rc_track_60_deg_up_to_flat;
    case TRACK_ELEM_FLAT_TO_60_DEG_DOWN:
        return vertical_drop_rc_track_flat_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_FLAT:
        return vertical_drop_rc_track_60_deg_down_to_flat;
    case TRACK_ELEM_BRAKE_FOR_DROP:
        return vertical_drop_rc_track_brake_for_drop;
    case TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP:
        return vertical_drop_rc_track_diag_flat_to_60_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
        return vertical_drop_rc_track_diag_60_deg_up_to_flat;
    case TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN:
        return vertical_drop_rc_track_diag_flat_to_60_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT:
        return vertical_drop_rc_track_diag_60_deg_down_to_flat;
    /* These track elements used to be specific to the steel vertical_drop */
    case TRACK_ELEM_HALF_LOOP_UP:
        return vertical_drop_rc_track_half_loop_up;
    case TRACK_ELEM_HALF_LOOP_DOWN:
        return vertical_drop_rc_track_half_loop_down;
    case TRACK_ELEM_LEFT_CORKSCREW_UP:
        return vertical_drop_rc_track_left_corkscrew_up;
    case TRACK_ELEM_RIGHT_CORKSCREW_UP:
        return vertical_drop_rc_track_right_corkscrew_up;
    case TRACK_ELEM_LEFT_CORKSCREW_DOWN:
        return vertical_drop_rc_track_left_corkscrew_down;
    case TRACK_ELEM_RIGHT_CORKSCREW_DOWN:
        return vertical_drop_rc_track_right_corkscrew_down;
    case TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE:
        return vertical_drop_rc_track_flat_to_60_deg_up_long_base;
    case TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE:
        return vertical_drop_rc_track_60_deg_up_to_flat_long_base;
    case TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE:
        return vertical_drop_rc_track_flat_to_60_deg_down_long_base;
    case TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122:
        return vertical_drop_rc_track_60_deg_up_to_flat_long_base122;
    case TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN:
        return vertical_drop_rc_track_left_barrel_roll_up_to_down;
    case TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN:
        return vertical_drop_rc_track_right_barrel_roll_up_to_down;
    case TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP:
        return vertical_drop_rc_track_left_barrel_roll_down_to_up;
    case TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP:
        return vertical_drop_rc_track_right_barrel_roll_down_to_up;
    case TRACK_ELEM_POWERED_LIFT:
        return vertical_drop_rc_track_powered_lift;
    case TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP:
        return vertical_drop_rc_track_left_large_half_loop_up;
    case TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP:
        return vertical_drop_rc_track_right_large_half_loop_up;
    case TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN:
        return vertical_drop_rc_track_right_large_half_loop_down;
    case TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN:
        return vertical_drop_rc_track_left_large_half_loop_down;
    case TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP:
        return vertical_drop_rc_track_90_deg_to_inverted_flat_quarter_loop_up;
    case TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN:
        return vertical_drop_rc_track_inverted_flat_to_90_deg_quarter_loop_down;

    case TRACK_ELEM_BOOSTER:
        return vertical_drop_rc_track_booster;
    }
    return NULL;
}
