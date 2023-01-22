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
namespace OpenRCT2
{
    enum class JuniorRCChainType : uint8_t
    {
        None,
        FrictionWheels,
        ChainLift,
    };

    struct Ride;
    struct PaintSession;
    struct TrackElement;

    void JuniorRCPaintStation(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, bool drawBlockBrake);

    void JuniorRCPaintTrackFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrack25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrack60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackFlatTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrack25DegUpTo60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrack60DegUpTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrack25DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);

    void JuniorRCPaintTrackLeftQuarterTurn5Tiles25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackRightQuarterTurn5Tiles25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);

    void JuniorRCPaintTrackRightQuarterTurn3Tiles25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackRightQuarterTurn3Tiles25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);

    void JuniorRCPaintTrackDiagFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiagFlatTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiagFlatTo60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag25DegUpTo60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag60DegUpTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag25DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag60DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiagFlatTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiagFlatTo60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag25DegDownTo60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag60DegDownTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag25DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
    void JuniorRCPaintTrackDiag60DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, uint16_t height,
        const TrackElement& trackElement, JuniorRCChainType chainType);
} // namespace OpenRCT2
