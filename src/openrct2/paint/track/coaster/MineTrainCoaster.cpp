/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

static constexpr uint32_t MINE_TRAIN_BLOCK_BRAKE_SW_NE_OPEN = 20060;
static constexpr uint32_t MINE_TRAIN_BLOCK_BRAKE_NW_SE_OPEN = 20061;
static constexpr uint32_t MINE_TRAIN_BLOCK_BRAKE_SW_NE_CLOSED = 20062;
static constexpr uint32_t MINE_TRAIN_BLOCK_BRAKE_NW_SE_CLOSED = 20063;

static constexpr uint32_t kMineTrainBlockBrakeImages[kNumOrthogonalDirections][2] = {
    { MINE_TRAIN_BLOCK_BRAKE_SW_NE_OPEN, MINE_TRAIN_BLOCK_BRAKE_SW_NE_CLOSED },
    { MINE_TRAIN_BLOCK_BRAKE_NW_SE_OPEN, MINE_TRAIN_BLOCK_BRAKE_NW_SE_CLOSED },
    { MINE_TRAIN_BLOCK_BRAKE_SW_NE_OPEN, MINE_TRAIN_BLOCK_BRAKE_SW_NE_CLOSED },
    { MINE_TRAIN_BLOCK_BRAKE_NW_SE_OPEN, MINE_TRAIN_BLOCK_BRAKE_NW_SE_CLOSED },
};

static constexpr const uint32_t kMinetrainRCDiagBrakeImages[kNumOrthogonalDirections] = {
    SPR_G2_MINETRAIN_DIAG_BRAKES,
    SPR_G2_MINETRAIN_DIAG_BRAKES + 1,
    SPR_G2_MINETRAIN_DIAG_BRAKES,
    SPR_G2_MINETRAIN_DIAG_BRAKES + 1,
};

static constexpr const uint32_t kMinetrainRCDiagBlockBrakeImages[2][kNumOrthogonalDirections] = {
    {
        SPR_G2_MINETRAIN_DIAG_BRAKES + 3,
        SPR_G2_MINETRAIN_DIAG_BRAKES + 5,
        SPR_G2_MINETRAIN_DIAG_BRAKES + 3,
        SPR_G2_MINETRAIN_DIAG_BRAKES + 5,
    },
    {
        SPR_G2_MINETRAIN_DIAG_BRAKES + 2,
        SPR_G2_MINETRAIN_DIAG_BRAKES + 4,
        SPR_G2_MINETRAIN_DIAG_BRAKES + 2,
        SPR_G2_MINETRAIN_DIAG_BRAKES + 4,
    },
};

// Magic number 4 refers to the number of track blocks in a diagonal track element
static constexpr const WoodenSupportSubType MineTrainRCDiagonalSupports[4][kNumOrthogonalDirections] = {
    { WoodenSupportSubType::Null, WoodenSupportSubType::Null, WoodenSupportSubType::Null, WoodenSupportSubType::Null },
    { WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner1, WoodenSupportSubType::Corner2,
      WoodenSupportSubType::Corner3 },
    { WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner0,
      WoodenSupportSubType::Corner1 },
    { WoodenSupportSubType::Null, WoodenSupportSubType::Null, WoodenSupportSubType::Null, WoodenSupportSubType::Null },
};

/** rct2: 0x0071BFA4 */
static void MineTrainRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20054), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20055), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20056), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20057), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20052), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20053), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
        }
    }
    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C154, 0x0071C164, 0x0071C174 */
static void MineTrainRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { 20064, SPR_STATION_BASE_B_SW_NE },
        { 20065, SPR_STATION_BASE_B_NW_SE },
        { 20064, SPR_STATION_BASE_B_SW_NE },
        { 20065, SPR_STATION_BASE_B_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][1]),
        { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 1 } });
    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        bool isClosed = trackElement.IsBrakeClosed();
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours.WithIndex(kMineTrainBlockBrakeImages[direction][isClosed]),
            { 0, 0, height }, { { 0, 0, height }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours.WithIndex(imageIds[direction][0]), { 0, 0, height },
            { { 0, 0, height }, { 32, 20, 1 } });
    }
    DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Boxed);
    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071BFB4 */
static void MineTrainRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20102), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20103), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20104), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20105), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20074), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20075), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20076), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20077), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
        }
    }

    DrawSupportForSequenceA<TrackElemType::Up25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0071BFC4 */
static void MineTrainRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20090), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 1:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20091), { 0, 0, height },
                { { 27, 0, height }, { 1, 32, 98 } });
            break;
        case 2:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20092), { 0, 0, height },
                { { 27, 0, height }, { 1, 32, 98 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20093), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
    }

    DrawSupportForSequenceA<TrackElemType::Up60>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x0071BFD4 */
static void MineTrainRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20094), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20095), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20096), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20097), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20066), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20067), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20068), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20069), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
        }
    }

    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x0071BFE4 */
static void MineTrainRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20078), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 1:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20079), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20082), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 66 } });
            break;
        case 2:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20080), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20083), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 66 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20081), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
    }

    DrawSupportForSequenceA<TrackElemType::Up25ToUp60>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x0071BFF4 */
static void MineTrainRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20084), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 1:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20085), { 0, 0, height },
                { { 4, 29, height - 16 }, { 24, 1, 61 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20088), { 0, 0, height },
                { { 0, 4, height }, { 32, 2, 66 } });
            break;
        case 2:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20086), { 0, 0, height },
                { { 4, 29, height - 16 }, { 24, 1, 61 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20089), { 0, 0, height },
                { { 0, 4, height }, { 32, 2, 66 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20087), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
    }

    DrawSupportForSequenceA<TrackElemType::Up60ToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x0071C004 */
static void MineTrainRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20098), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20099), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20100), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20101), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20070), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20071), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20072), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20073), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                break;
        }
    }

    DrawSupportForSequenceA<TrackElemType::Up25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x0071C014 */
static void MineTrainRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C024 */
static void MineTrainRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C034 */
static void MineTrainRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C044 */
static void MineTrainRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C054 */
static void MineTrainRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C064 */
static void MineTrainRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C0B4 */
static void MineTrainRCTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20155), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20160), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20165), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20150), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20154), { 0, 0, height }, { 32, 16, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20159), { 0, 0, height }, { 32, 16, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20164), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20149), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20153), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20158), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20163), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20148), { 0, 0, height }, { 16, 16, 1 });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre,
                        PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20152), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20157), { 0, 0, height }, { 16, 32, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20162), { 0, 0, height }, { 16, 32, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20147), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20151), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20156), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20161), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20146), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C0C4 */
static void MineTrainRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C074 */
static void MineTrainRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20106), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20114), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20107), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20115), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20108), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20109), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
    }
    DrawSupportForSequenceA<TrackElemType::FlatToLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C084 */
static void MineTrainRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20110), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20111), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20112), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20116), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20113), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20117), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
    }
    DrawSupportForSequenceA<TrackElemType::FlatToRightBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C094 */
static void MineTrainRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20112), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20116), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20113), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20117), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20110), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20111), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
    }
    DrawSupportForSequenceA<TrackElemType::LeftBankToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C0A4 */
static void MineTrainRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20108), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20109), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20106), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20114), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20107), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20115), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
    }
    DrawSupportForSequenceA<TrackElemType::RightBankToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C0D4 */
static void MineTrainRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20175), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20186), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20180), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20185), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20170), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::BankedLeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20174), { 0, 0, height }, { 32, 16, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20179), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20184), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20169), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::BankedLeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20173), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20178), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20183), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20168), { 0, 0, height }, { 16, 16, 1 });
                    break;
            }

            DrawSupportForSequenceA<TrackElemType::BankedLeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre,
                        PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20172), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20177), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20182), { 0, 0, height }, { 16, 32, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20167), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::BankedLeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20171), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20176), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20181), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20187), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20166), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::BankedLeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C0E4 */
static void MineTrainRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackBankedLeftQuarterTurn5(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C204 */
static void MineTrainRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20118), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20122), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20119), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20123), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20120), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20121), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
    }

    DrawSupportForSequenceA<TrackElemType::LeftBankToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x0071C214 */
static void MineTrainRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20124), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20125), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20126), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20128), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20127), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20129), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
    }

    DrawSupportForSequenceA<TrackElemType::RightBankToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x0071C224 */
static void MineTrainRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20130), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20134), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20131), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20135), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20132), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20133), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
    }

    DrawSupportForSequenceA<TrackElemType::Up25ToLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x0071C234 */
static void MineTrainRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20136), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20137), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20138), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20140), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20139), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20141), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
    }

    DrawSupportForSequenceA<TrackElemType::Up25ToRightBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x0071C244 */
static void MineTrainRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C254 */
static void MineTrainRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C264 */
static void MineTrainRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C274 */
static void MineTrainRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C304 */
static void MineTrainRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20142), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20143), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20144), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20145), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            break;
    }
    DrawSupportForSequenceA<TrackElemType::LeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C314 */
static void MineTrainRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C0F4 */
static void MineTrainRCTrackLeftQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20250), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20255), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20260), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20265), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    break;
            }

            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5TilesUp25>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20251), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20256), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20261), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20266), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5TilesUp25>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20252), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20257), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20262), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20267), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5TilesUp25>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre,
                        PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20253), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20258), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20263), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20268), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5TilesUp25>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20254), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 3);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20259), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 0);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20264), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 1);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20269), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 2);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C104 */
static void MineTrainRCTrackRightQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20230), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20235), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20240), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20245), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    break;
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20231), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20236), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20241), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20246), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20232), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20237), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20242), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20247), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::centre,
                        PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20233), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20238), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20243), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20248), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20234), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 1);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20239), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 2);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20244), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 3);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20249), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 0);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C114 */
static void MineTrainRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackRightQuarterTurn525DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C124 */
static void MineTrainRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn525DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C134 */
static void MineTrainRCTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20270), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20274), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20273), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20277), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20271), { 0, 0, height }, { 32, 26, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20275), { 0, 0, height }, { 32, 26, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20272), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20276), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20272), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20276), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20271), { 0, 0, height }, { 32, 26, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20275), { 0, 0, height }, { 32, 26, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20273), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20277), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20270), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20274), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C144 */
static void MineTrainRCTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20278), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20282), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20281), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20285), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20279), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20283), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20280), { 0, 0, height }, { 32, 26, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20284), { 0, 0, height }, { 32, 26, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20280), { 0, 0, height }, { 32, 26, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20284), { 0, 0, height }, { 32, 26, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20279), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20283), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20281), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20285), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20278), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20282), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C184 */
static void MineTrainRCTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20193), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20196), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20199), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20190), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20192), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20195), { 0, 0, height }, { 16, 16, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20198), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20189), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20191), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20194), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20197), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20188), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C194 */
static void MineTrainRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C1A4 */
static void MineTrainRCTrackLeftQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20205), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20212), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20208), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20211), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20202), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20204), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20207), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20210), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20201), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20203), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20206), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20209), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20213), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20200), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C1B4 */
static void MineTrainRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C1C4 */
static void MineTrainRCTrackLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20225), { 0, 6, height }, { 32, 20, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20227), { 0, 6, height }, { 32, 20, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20229), { 0, 6, height }, { 32, 20, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20223), { 0, 6, height }, { 32, 20, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20224), { 6, 0, height }, { 20, 32, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20226), { 6, 0, height }, { 20, 32, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20228), { 6, 0, height }, { 20, 32, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20222), { 6, 0, height }, { 20, 32, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C1D4 */
static void MineTrainRCTrackRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20214), { 0, 6, height }, { 32, 20, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20216), { 0, 6, height }, { 32, 20, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20218), { 0, 6, height }, { 32, 20, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20220), { 0, 6, height }, { 32, 20, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20215), { 6, 0, height }, { 20, 32, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20217), { 6, 0, height }, { 20, 32, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20219), { 6, 0, height }, { 20, 32, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20221), { 6, 0, height }, { 20, 32, 1 });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C1E4 */
static void MineTrainRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackRightQuarterTurn325DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C1F4 */
static void MineTrainRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn325DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C284 */
static void MineTrainRCTrackLeftHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20305), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20312), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20308), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20311), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20302), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20304), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20307), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20310), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20301), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20303), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20306), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20309), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20313), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20300), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20302), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20305), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20312), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20308), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20311), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20301), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20304), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20307), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20310), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20300), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20303), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20306), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20309), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20313), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::bottomCorner,
                        PaintSegment::topLeftSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C294 */
static void MineTrainRCTrackRightHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20286), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20289), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20292), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20295), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20299), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::bottomCorner,
                        PaintSegment::topLeftSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20287), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20290), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20293), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20296), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20288), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20291), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20298), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20294), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20297), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20289), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20292), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20295), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20299), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20286), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20290), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20293), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20296), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20287), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20291), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20298), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20294), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20297), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20288), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C2A4 */
static void MineTrainRCTrackLeftHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackRightHalfBankedHelixUpSmall(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C2B4 */
static void MineTrainRCTrackRightHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftHalfBankedHelixUpSmall(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C2C4 */
static void MineTrainRCTrackLeftHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20345), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20356), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20350), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20355), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20340), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20344), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20349), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20354), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20339), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20343), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20348), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20178), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20353), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20338), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::bottomCorner, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20342), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20347), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20177), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20352), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20337), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20341), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20346), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20351), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20357), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20336), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20340), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20345), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20356), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20350), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20355), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 8:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20339), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20344), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20349), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20354), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 10:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20338), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20343), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20348), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20178), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20353), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 11:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20337), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20342), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20347), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20177), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20352), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 13:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20336), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20341), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20346), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20351), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20357), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::bottomCorner,
                        PaintSegment::topLeftSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C2D4 */
static void MineTrainRCTrackRightHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20314), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20319), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20324), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20329), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20335), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::bottomCorner,
                        PaintSegment::topLeftSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20315), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20320), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20325), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20330), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20316), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20321), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20326), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20331), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20317), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20322), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20327), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20332), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20318), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20323), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20334), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20328), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20333), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20319), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20324), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20329), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20335), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20314), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 8:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20320), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20325), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20330), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20315), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 10:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20321), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20326), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20331), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20316), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::bottomCorner, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 11:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20322), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20327), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20332), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20317), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 13:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20323), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20334), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20328), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20333), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20318), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C2E4 */
static void MineTrainRCTrackLeftHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackRightHalfBankedHelixUpLarge(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C2F4 */
static void MineTrainRCTrackRightHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackLeftHalfBankedHelixUpLarge(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C324 */
static void MineTrainRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20058), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20059), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C334 */
static void MineTrainRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20052), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20053), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20052), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(20053), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
            break;
    }
    TrackPaintUtilOnridePhotoPaint2(session, direction, trackElement, height, kGeneralSupportHeightOnRidePhoto, 2);
}

/** rct2: 0x0071C354 */
static void MineTrainRCTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20452), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20456), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20460), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20464), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20453), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20457), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20461), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20465), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20454), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20458), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20462), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20466), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner3, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20455), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20459), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20463), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20467), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C364 */
static void MineTrainRCTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20436), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20440), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20444), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20448), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20437), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20441), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20445), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20449), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20438), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20442), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20446), { 0, 0, height },
                        { { 4, 4, height }, { 28, 28, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20450), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20439), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20443), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20447), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20451), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C374 */
static void MineTrainRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C384 */
static void MineTrainRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C394 */
static void MineTrainRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20484), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20488), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20492), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20496), { 0, 0, height },
                        { { 0, 27, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20485), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20489), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 34, 16, 0 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20493), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20497), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20486), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20490), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20494), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20498), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner3, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20487), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20491), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 18, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20495), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20499), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C3A4 */
static void MineTrainRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20468), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20472), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20476), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20480), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20469), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20473), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20477), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 34, 16, 0 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20481), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20470), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20474), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20478), { 0, 0, height },
                        { { 4, 4, height + 27 }, { 28, 28, 0 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20482), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20471), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20475), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20479), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 18, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20483), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C3B4 */
static void MineTrainRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C3C4 */
static void MineTrainRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C344 */
static void MineTrainRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    constexpr ImageIndex images[2][kNumOrthogonalDirections] = {
        { 20358, 20359, 20360, 20361 },
        { 20386, 20387, 20388, 20389 },
    };

    TrackPaintUtilDiagTilesPaint(
        session, 1, height, direction, trackSequence, images[trackElement.HasChain()], defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    switch (trackSequence)
    {
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
            break;
        case 2:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void MineTrainRCTrackDiagBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, 1, height, direction, trackSequence, kMinetrainRCDiagBrakeImages, defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    if (MineTrainRCDiagonalSupports[trackSequence][direction] != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetup(
            session, supportType.wooden, MineTrainRCDiagonalSupports[trackSequence][0], height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void MineTrainRCTrackDiagBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, 13, height, direction, trackSequence, kMinetrainRCDiagBlockBrakeImages[trackElement.IsBrakeClosed()],
        defaultDiagTileOffsets, defaultDiagBoundLengths, nullptr);

    if (MineTrainRCDiagonalSupports[trackSequence][direction] != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetup(
            session, supportType.wooden, MineTrainRCDiagonalSupports[trackSequence][direction], height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C414 */
static void MineTrainRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20401 : 20373;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            break;
        case 1:
            if (direction == 0)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20398 : 20370;
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            break;
        case 2:
            if (direction == 2)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20400 : 20372;
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            break;
        case 3:
            if (direction == 1)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20399 : 20371;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            break;
    }

    DrawSupportForSequenceA<TrackElemType::DiagUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0071C474 */
static void MineTrainRCTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20385), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20382), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20384), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20383), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
    }
}

/** rct2: 0x0071C3F4 */
static void MineTrainRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20393 : 20365;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (direction == 0)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20390 : 20362;
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (direction == 2)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20392 : 20364;
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (direction == 1)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20391 : 20363;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x0071C454 */
static void MineTrainRCTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20377), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20374), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20376), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20375), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C464 */
static void MineTrainRCTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20381), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20378), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20380), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20379), { -16, -16, height },
                    { { 0, 0, height }, { 16, 16, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C404 */
static void MineTrainRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20397 : 20369;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            if (direction == 0)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20394 : 20366;
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            if (direction == 2)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20396 : 20368;
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            if (direction == 1)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20395 : 20367;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x0071C444 */
static void MineTrainRCTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20399 : 20371;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            if (direction == 0)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20400 : 20372;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            if (direction == 2)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20398 : 20370;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            if (direction == 1)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20401 : 20373;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x0071C4A4 */
static void MineTrainRCTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20383), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20384), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20382), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20385), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
    }
}

/** rct2: 0x0071C424 */
static void MineTrainRCTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20395 : 20367;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 1:
            if (direction == 0)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20396 : 20368;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 2:
            if (direction == 2)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20394 : 20366;
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 3:
            if (direction == 1)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20397 : 20369;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0071C484 */
static void MineTrainRCTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20379), { -16, -16, height },
                    { { 0, 0, height }, { 16, 16, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20380), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20378), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20381), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C494 */
static void MineTrainRCTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20375), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20376), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20374), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20377), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C434 */
static void MineTrainRCTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20391 : 20363;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (direction == 0)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20392 : 20364;
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (direction == 2)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20390 : 20362;
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (direction == 1)
            {
                ImageIndex imageIndex = trackElement.HasChain() ? 20393 : 20365;
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(imageIndex), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x0071C4D4 */
static void MineTrainRCTrackDiagFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20409), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20406), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20410), { -16, -16, height },
                    { { -16, -16, height + 27 }, { 32, 32, 0 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20408), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20407), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C4B4 */
static void MineTrainRCTrackDiagFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20414), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20411), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20413), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20415), { -16, -16, height },
                    { { -16, -16, height + 27 }, { 32, 32, 0 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20412), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C4C4 */
static void MineTrainRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20412), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20413), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20415), { -16, -16, height },
                    { { -16, -16, height + 27 }, { 32, 32, 0 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20411), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20414), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C4E4 */
static void MineTrainRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20407), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20408), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20406), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20410), { -16, -16, height },
                    { { -16, -16, height + 27 }, { 32, 32, 0 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20409), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C514 */
static void MineTrainRCTrackDiagLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20429), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20426), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20430), { -16, -16, height },
                    { { -16, -16, height + 35 }, { 32, 32, 0 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20428), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20427), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x0071C524 */
static void MineTrainRCTrackDiagRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20434), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20431), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20433), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20435), { -16, -16, height },
                    { { -16, -16, height + 35 }, { 32, 32, 0 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20432), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x0071C4F4 */
static void MineTrainRCTrackDiag25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20419), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20416), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20420), { -16, -16, height },
                    { { -16, -16, height + 35 }, { 32, 32, 0 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20418), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20417), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x0071C504 */
static void MineTrainRCTrackDiag25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20424), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20421), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20423), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20425), { -16, -16, height },
                    { { -16, -16, height + 35 }, { 32, 32, 0 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20422), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x0071C534 */
static void MineTrainRCTrackDiagLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(20422), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20423), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20425), { -16, -16, height },
                    { { -16, -16, height + 35 }, { 32, 32, 0 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20421), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20424), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0071C544 */
static void MineTrainRCTrackDiagRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20417), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20418), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20416), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20420), { -16, -16, height },
                    { { -16, -16, height + 35 }, { 32, 32, 0 } });
            }

            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);

            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20419), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0071C554 */
static void MineTrainRCTrackDiag25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20432), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20433), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20435), { -16, -16, height },
                    { { -16, -16, height + 35 }, { 32, 32, 0 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20431), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20434), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x0071C564 */
static void MineTrainRCTrackDiag25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20427), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20428), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20426), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20430), { -16, -16, height },
                    { { -16, -16, height + 35 }, { 32, 32, 0 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20429), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 1 } });
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x0071C3D4 */
static void MineTrainRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20405), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 3 } });
            }

            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20402), { -16, -16, height },
                    { { -16, -16, height + 27 }, { 32, 32, 0 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20404), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 3 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20403), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 3 } });
            }

            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(BlockedSegments::kDiagStraightFlat[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C3E4 */
static void MineTrainRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20403), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 3 } });
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20404), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 3 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParent(
                    session, session.TrackColours.WithIndex(20402), { -16, -16, height },
                    { { -16, -16, height + 27 }, { 32, 32, 0 } });
            }

            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            if (direction == 1)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(20405), { -16, -16, height },
                    { { -16, -16, height }, { 32, 32, 3 } });
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C574 */
static void MineTrainRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isClosed = trackElement.IsBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(kMineTrainBlockBrakeImages[direction][isClosed]), { 0, 0, height },
        { { 0, 6, height }, { 32, 20, 1 } });
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void MineTrainRCTrackFlatTo60DegUpLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 0),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 4),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 8),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 12),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq0);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 1),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 5),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 9),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 13),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq1);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 2),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 6),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 10),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 14),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq2);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 3),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 7),
                        { 0, 0, height }, { { 28, 4, height }, { 2, 24, 56 } });
                    break;
                case 2:
                    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 11),
                        { 0, 0, height }, { { 28, 4, height }, { 2, 24, 56 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 15),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq3);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}
static void MineTrainRCTrack60DegUpToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 16),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 20),
                        { 0, 0, height }, { { 28, 4, height }, { 2, 24, 56 } });
                    break;
                case 2:
                    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 24),
                        { 0, 0, height }, { { 28, 4, height }, { 2, 24, 56 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 28),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq0);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 17),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 21),
                        { 0, 0, height }, { { 28, 4, height }, { 2, 24, 56 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 25),
                        { 0, 0, height }, { { 28, 4, height }, { 2, 24, 56 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 29),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq1);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 18),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 22),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 26),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 30),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq2);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 19),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 23),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 27),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 31),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq3);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void MineTrainRCTrackFlatTo60DegDownLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack60DegUpToFlatLongBase(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void MineTrainRCTrack60DegDownToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackFlatTo60DegUpLongBase(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionMineTrainRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return MineTrainRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return MineTrainRCTrackStation;
        case TrackElemType::Up25:
            return MineTrainRCTrack25DegUp;
        case TrackElemType::Up60:
            return MineTrainRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return MineTrainRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return MineTrainRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return MineTrainRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return MineTrainRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return MineTrainRCTrack25DegDown;
        case TrackElemType::Down60:
            return MineTrainRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return MineTrainRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return MineTrainRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return MineTrainRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return MineTrainRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return MineTrainRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return MineTrainRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return MineTrainRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return MineTrainRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return MineTrainRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return MineTrainRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return MineTrainRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return MineTrainRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return MineTrainRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return MineTrainRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return MineTrainRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return MineTrainRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return MineTrainRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return MineTrainRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return MineTrainRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return MineTrainRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return MineTrainRCTrackLeftBank;
        case TrackElemType::RightBank:
            return MineTrainRCTrackRightBank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return MineTrainRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return MineTrainRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return MineTrainRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return MineTrainRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::SBendLeft:
            return MineTrainRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return MineTrainRCTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return MineTrainRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return MineTrainRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return MineTrainRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return MineTrainRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return MineTrainRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return MineTrainRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return MineTrainRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return MineTrainRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return MineTrainRCTrackLeftHalfBankedHelixUpSmall;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return MineTrainRCTrackRightHalfBankedHelixUpSmall;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return MineTrainRCTrackLeftHalfBankedHelixDownSmall;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return MineTrainRCTrackRightHalfBankedHelixDownSmall;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return MineTrainRCTrackLeftHalfBankedHelixUpLarge;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return MineTrainRCTrackRightHalfBankedHelixUpLarge;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return MineTrainRCTrackLeftHalfBankedHelixDownLarge;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return MineTrainRCTrackRightHalfBankedHelixDownLarge;
        case TrackElemType::Brakes:
            return MineTrainRCTrackBrakes;
        case TrackElemType::OnRidePhoto:
            return MineTrainRCTrackOnRidePhoto;
        case TrackElemType::LeftEighthToDiag:
            return MineTrainRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return MineTrainRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return MineTrainRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return MineTrainRCTrackRightEighthToOrthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return MineTrainRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return MineTrainRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return MineTrainRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return MineTrainRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlat:
            return MineTrainRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return MineTrainRCTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return MineTrainRCTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return MineTrainRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return MineTrainRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return MineTrainRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return MineTrainRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return MineTrainRCTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return MineTrainRCTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return MineTrainRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return MineTrainRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return MineTrainRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return MineTrainRCTrackDiag25DegDownToFlat;
        case TrackElemType::DiagFlatToLeftBank:
            return MineTrainRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return MineTrainRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return MineTrainRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return MineTrainRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return MineTrainRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return MineTrainRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return MineTrainRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return MineTrainRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return MineTrainRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return MineTrainRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return MineTrainRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return MineTrainRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return MineTrainRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return MineTrainRCTrackDiagRightBank;
        case TrackElemType::BlockBrakes:
            return MineTrainRCTrackBlockBrakes;
        case TrackElemType::DiagBrakes:
            return MineTrainRCTrackDiagBrakes;
        case TrackElemType::DiagBlockBrakes:
            return MineTrainRCTrackDiagBlockBrakes;
        case TrackElemType::FlatToUp60LongBase:
            return MineTrainRCTrackFlatTo60DegUpLongBase;
        case TrackElemType::Up60ToFlatLongBase:
            return MineTrainRCTrack60DegUpToFlatLongBase;
        case TrackElemType::FlatToDown60LongBase:
            return MineTrainRCTrackFlatTo60DegDownLongBase;
        case TrackElemType::Down60ToFlatLongBase:
            return MineTrainRCTrack60DegDownToFlatLongBase;
        default:
            return TrackPaintFunctionDummy;
    }
}
