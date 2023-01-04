/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../common.h"

enum class JuniorRcChainType : uint8_t
{
    None,
    FrictionWheels,
    ChainLift,
};

struct Ride;
struct PaintSession;
struct TrackElement;

void junior_rc_paint_station(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, bool drawBlockBrake);

void junior_rc_paint_track_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_flat_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_25_deg_up_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_60_deg_up_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);

void junior_rc_paint_track_left_quarter_turn_5_tiles_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_right_quarter_turn_5_tiles_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);

void junior_rc_paint_track_right_quarter_turn_3_tiles_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_right_quarter_turn_3_tiles_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);

void junior_rc_paint_track_diag_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_flat_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_flat_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_25_deg_up_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_60_deg_up_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_60_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_flat_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_flat_to_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_25_deg_down_to_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_60_deg_down_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void junior_rc_paint_track_diag_60_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
