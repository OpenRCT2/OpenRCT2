/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../common.h"

struct paint_session;
struct rct_tile_element;

void bolliger_mabillard_track_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_station(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_up_to_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_60_deg_up_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_up_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_down_to_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_60_deg_down_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_down_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_5(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_5(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_left_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_right_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_bank_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_bank_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_banked_left_quarter_turn_5(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_banked_right_quarter_turn_5(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_bank_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_bank_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_up_to_left_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_up_to_right_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_bank_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_bank_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_down_to_left_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_down_to_right_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_5_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_5_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_5_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_5_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_s_bend_left(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_s_bend_right(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_vertical_loop(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_vertical_loop(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_3(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_3(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_3_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_3_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_3_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_3_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_3_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_3_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_half_banked_helix_up_small(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_half_banked_helix_up_small(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_half_banked_helix_down_small(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_half_banked_helix_down_small(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_half_banked_helix_up_large(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_half_banked_helix_up_large(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_half_banked_helix_down_large(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_half_banked_helix_down_large(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_1_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_1_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_1_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_1_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_brakes(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_up_left_banked(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_up_right_banked(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_on_ride_photo(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_down_left_banked(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_down_right_banked(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_90_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_90_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_60_deg_up_to_90_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_90_deg_down_to_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_90_deg_up_to_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_60_deg_down_to_90_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_eighth_to_diag(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_eighth_to_diag(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_eighth_to_orthogonal(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_eighth_to_orthogonal(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_eighth_bank_to_diag(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_eighth_bank_to_diag(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_eighth_bank_to_orthogonal(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_eighth_bank_to_orthogonal(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_flat_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_25_deg_up_to_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_60_deg_up_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_25_deg_up_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_flat_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_25_deg_down_to_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_60_deg_down_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_25_deg_down_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_flat_to_left_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_flat_to_right_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_left_bank_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_right_bank_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_left_bank_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_right_bank_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_25_deg_up_to_left_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_25_deg_up_to_right_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_left_bank_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_right_bank_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_25_deg_down_to_left_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_25_deg_down_to_right_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_left_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_right_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_bank_to_left_quarter_turn_3_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_bank_to_right_quarter_turn_3_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_3_25_deg_down_to_left_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_3_25_deg_down_to_right_bank(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_block_brakes(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_quarter_turn_3_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_quarter_turn_3_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_quarter_turn_5_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_quarter_turn_5_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_quarter_turn_5_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_up_to_left_banked_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_up_to_right_banked_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_25_deg_up_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_25_deg_up_to_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_down_to_left_banked_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_25_deg_down_to_right_banked_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_25_deg_down_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_25_deg_down_to_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_flat_to_left_banked_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_flat_to_right_banked_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_25_deg_up_to_left_banked_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_25_deg_up_to_right_banked_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_flat_to_left_banked_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_flat_to_right_banked_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_25_deg_down_to_left_banked_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_25_deg_down_to_right_banked_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_left_banked_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_right_banked_25_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_25_deg_up_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_25_deg_up_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_left_banked_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_right_banked_25_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_banked_25_deg_down_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_banked_25_deg_down_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_1_90_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_1_90_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_quarter_turn_1_90_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_quarter_turn_1_90_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);

/* Previously specific to the Vertical RC */
void bolliger_mabillard_track_flat_to_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_60_deg_up_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_60_deg_down_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_brake_for_drop(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_flat_to_60_deg_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_60_deg_up_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_flat_to_60_deg_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_diag_60_deg_down_to_flat(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);

/* Previously specific to the Steel Twister */
void bolliger_mabillard_track_half_loop_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_half_loop_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_corkscrew_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_corkscrew_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_corkscrew_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_corkscrew_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_60_deg_up_long_base(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_60_deg_up_to_flat_long_base(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_flat_to_60_deg_down_long_base(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_60_deg_up_to_flat_long_base122(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_barrel_roll_up_to_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_barrel_roll_up_to_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_barrel_roll_down_to_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_barrel_roll_down_to_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_powered_lift(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_large_half_loop_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_large_half_loop_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_right_large_half_loop_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_left_large_half_loop_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_90_deg_to_inverted_flat_quarter_loop_up(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
void bolliger_mabillard_track_inverted_flat_to_90_deg_quarter_loop_down(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);

void bolliger_mabillard_track_booster(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   supportType);
