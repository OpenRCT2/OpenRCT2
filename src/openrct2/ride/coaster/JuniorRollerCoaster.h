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

void JuniorRcPaintStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, bool drawBlockBrake);

void JuniorRcPaintTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);

void JuniorRcPaintTrackLeftQuarterTurn5Tiles25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackRightQuarterTurn5Tiles25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);

void juniorRcPaintTrackRightQuarterTurn3Tiles25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackRightQuarterTurn3Tiles25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);

void JuniorRcPaintTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiagFlatTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag60DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiagFlatTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
void JuniorRcPaintTrackDiag60DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
    const TrackElement& trackElement, JuniorRcChainType chainType);
