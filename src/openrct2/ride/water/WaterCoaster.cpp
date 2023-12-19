/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../common.h"
#include "../Ride.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../coaster/JuniorRollerCoaster.h"

static void WaterRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrackFlat(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrack25DegUp(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrackFlatTo25DegUp(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrack25DegUpToFlat(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WaterRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void WaterRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WaterRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void WaterRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WaterRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void WaterRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrackDiagFlat(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrackDiag25DegUp(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrackDiagFlatTo25DegUp(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrackDiag25DegUpToFlat(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrackDiag25DegDown(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrackDiagFlatTo25DegDown(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRCPaintTrackDiag25DegDownToFlat(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRCChainType::ChainLift : JuniorRCChainType::None);
}

static void WaterRCTrackStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintStation(session, ride, trackSequence, direction, height, trackElement, false);
}

static void WaterRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrack60DegUp(session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static void WaterRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrack25DegUpTo60DegUp(session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static void WaterRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrack60DegUpTo25DegUp(session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static void WaterRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WaterRCTrack60DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

static void WaterRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WaterRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

static void WaterRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WaterRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

// 5 tile turns

static void WaterRCTrackLeftQuarterTurn5Tiles25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrackLeftQuarterTurn5Tiles25DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static void WaterRCTrackRightQuarterTurn5Tiles25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrackRightQuarterTurn5Tiles25DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static constexpr uint8_t water_rc_left_quarter_turn_5_tiles_to_right_turn_map[] = {
    6, 4, 5, 3, 1, 2, 0,
};

static void WaterRCTrackLeftQuarterTurn5Tiles25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WaterRCTrackRightQuarterTurn5Tiles25DegUp(
        session, ride, water_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence], (direction + 1) % 4, height,
        trackElement);
}

static void WaterRCTrackRightQuarterTurn5Tiles25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WaterRCTrackLeftQuarterTurn5Tiles25DegUp(
        session, ride, water_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence], (direction + 3) % 4, height,
        trackElement);
}

// 3 tile turns

static void WaterRCTrackRightQuarterTurn3Tiles25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrackRightQuarterTurn3Tiles25DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static void WaterRCTrackRightQuarterTurn3Tiles25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrackRightQuarterTurn3Tiles25DegDown(
        session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static constexpr uint8_t water_rc_left_quarter_turn_3_tiles_to_right_turn_map[] = {
    3,
    1,
    2,
    0,
};

static void WaterRCTrackLeftQuarterTurn3Tiles25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = water_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    WaterRCTrackRightQuarterTurn3Tiles25DegDown(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

static void WaterRCTrackLeftQuarterTurn3Tiles25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = water_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    WaterRCTrackRightQuarterTurn3Tiles25DegUp(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

static void WaterRCTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrackDiag60DegUp(session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static void WaterRCTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrackDiag25DegUpTo60DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static void WaterRCTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrackDiag60DegUpTo25DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static void WaterRCTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrackDiag60DegDown(session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static void WaterRCTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrackDiag25DegDownTo60DegDown(
        session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

static void WaterRCTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRCPaintTrackDiag60DegDownTo25DegDown(
        session, ride, trackSequence, direction, height, trackElement, JuniorRCChainType::None);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionWaterRC(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return WaterRCTrackFlat;
        case TrackElemType::Up25:
            return WaterRCTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return WaterRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return WaterRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return WaterRCTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return WaterRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return WaterRCTrack25DegDownToFlat;
        case TrackElemType::DiagFlat:
            return WaterRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return WaterRCTrackDiag25DegUp;
        case TrackElemType::DiagFlatToUp25:
            return WaterRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return WaterRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return WaterRCTrackDiag25DegDown;
        case TrackElemType::DiagFlatToDown25:
            return WaterRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return WaterRCTrackDiag25DegDownToFlat;

        // Use Junior RC without lift hill
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return WaterRCTrackStation;
        case TrackElemType::Up60:
            return WaterRCTrack60DegUp;
        case TrackElemType::Up25ToUp60:
            return WaterRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return WaterRCTrack60DegUpTo25DegUp;
        case TrackElemType::Down60:
            return WaterRCTrack60DegDown;
        case TrackElemType::Down25ToDown60:
            return WaterRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return WaterRCTrack60DegDownTo25DegDown;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return WaterRCTrackLeftQuarterTurn5Tiles25DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return WaterRCTrackRightQuarterTurn5Tiles25DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return WaterRCTrackLeftQuarterTurn5Tiles25DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return WaterRCTrackRightQuarterTurn5Tiles25DegDown;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return WaterRCTrackLeftQuarterTurn3Tiles25DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return WaterRCTrackRightQuarterTurn3Tiles25DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return WaterRCTrackLeftQuarterTurn3Tiles25DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return WaterRCTrackRightQuarterTurn3Tiles25DegDown;
        case TrackElemType::DiagUp60:
            return WaterRCTrackDiag60DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return WaterRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return WaterRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagDown60:
            return WaterRCTrackDiag60DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return WaterRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return WaterRCTrackDiag60DegDownTo25DegDown;
    }
    return GetTrackPaintFunctionJuniorRC(trackType);
}
