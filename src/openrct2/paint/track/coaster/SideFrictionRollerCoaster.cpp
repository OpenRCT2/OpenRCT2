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

constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_DIR_0_B = 21658;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_DIR_1_A = 21647;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_DIR_1_B = 21659;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_DIR_2_A = 21648;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_DIR_0_A = 21646;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_DIR_2_B = 21660;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_DIR_3_A = 21649;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_DIR_3_B = 21661;

constexpr int32_t SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_0_A = 21638;
constexpr int32_t SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_0_B = 21650;
constexpr int32_t SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_1_A = 21639;
constexpr int32_t SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_1_B = 21651;
constexpr int32_t SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_2_A = 21640;
constexpr int32_t SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_2_B = 21652;
constexpr int32_t SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_3_A = 21641;
constexpr int32_t SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_3_B = 21653;

constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_0_A = 21642;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_0_B = 21654;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_1_A = 21643;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_1_B = 21655;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_2_A = 21644;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_2_B = 21656;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_3_A = 21645;
constexpr int32_t SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_3_B = 21657;

constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_0_A = 21882;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_0_B = 21886;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_1_A = 21883; // Needs no B piece
constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_2_A = 21884;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_2_B = 21887;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_3_A = 21885; // Needs no B piece

// Whole block appears to lack B counterparts
constexpr int32_t SPR_SIDE_FRICTION_DIAG_25_DEG_UP_TO_60_DEG_UP_DIR_0_A = 21870;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_25_DEG_UP_TO_60_DEG_UP_DIR_1_A = 21871;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_25_DEG_UP_TO_60_DEG_UP_DIR_2_A = 21872;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_25_DEG_UP_TO_60_DEG_UP_DIR_3_A = 21873;

constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_0_A = 21876;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_0_B = 21880;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_1_A = 21877; // Needs no B piece
constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_2_A = 21878;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_2_B = 21881;
constexpr int32_t SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_3_A = 21879; // Needs no B piece

/** rct2: 0x0077839C */
static void SideFrictionRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21662), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21664), { 0, 0, height },
                    { { 0, 2, height + 27 }, { 32, 27, 0 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21663), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21665), { 0, 0, height },
                    { { 0, 2, height + 27 }, { 32, 27, 0 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21666), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21668), { 0, 0, height },
                    { { 0, 2, height + 27 }, { 32, 27, 0 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21667), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21669), { 0, 0, height },
                    { { 0, 2, height + 27 }, { 32, 27, 0 } });
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
                    session, direction, session.TrackColours.WithIndex(21606), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21608), { 0, 0, height },
                    { { 0, 2, height + 27 }, { 32, 27, 0 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21607), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21609), { 0, 0, height },
                    { { 0, 2, height + 27 }, { 32, 27, 0 } });
                break;
        }
    }
    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007784AC, 0x007784BC, 0x007784CC */
static void SideFrictionRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4] = {
        21610,
        21611,
        21610,
        21611,
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(imageIds[direction]), { 0, 0, height },
        { { 0, 2, height }, { 32, 27, 2 } });
    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, 9, 11);
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007783AC */
static void SideFrictionRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21678), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21690), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21679), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21691), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 20 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21680), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21692), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 20 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21681), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21693), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21622), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21634), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21623), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21635), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 20 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21624), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21636), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 20 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21625), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21637), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
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

/** rct2: 0x007783CC */
static void SideFrictionRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21670), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21682), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21671), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21683), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 16 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21672), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21684), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 16 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21673), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21685), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21614), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21626), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21615), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21627), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 16 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21616), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21628), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 16 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21617), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21629), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
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

/** rct2: 0x007783FC */
static void SideFrictionRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21674), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21686), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21675), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21687), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 16 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21676), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21688), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 16 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21677), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21689), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21618), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21630), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21619), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21631), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 16 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21620), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21632), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 16 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21621), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21633), { 0, 0, height },
                    { { 0, 26, height + 5 }, { 32, 1, 9 } });
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

/** rct2: 0x0077840C */
static void SideFrictionRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SideFrictionRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0077842C */
static void SideFrictionRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SideFrictionRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0077845C */
static void SideFrictionRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SideFrictionRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0077846C */
static void SideFrictionRCTrackLeftQuarterTurn5(
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
                        session, direction, session.TrackColours.WithIndex(21727), { 0, 2, height }, { 32, 27, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21747), { 0, 2, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21732), { 0, 2, height }, { 32, 27, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21752), { 0, 2, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21737), { 0, 2, height }, { 32, 32, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21757), { 0, 2, height },
                        { { 0, 2, height + 27 }, { 32, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21722), { 0, 2, height }, { 32, 32, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21742), { 0, 2, height },
                        { { 0, 2, height + 27 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(21726), { 0, 0, height }, { 32, 16, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21746), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21731), { 0, 0, height }, { 32, 16, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21751), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21736), { 0, 16, height }, { 32, 16, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21756), { 0, 16, height },
                        { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21721), { 0, 16, height }, { 32, 16, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21741), { 0, 16, height },
                        { { 0, 16, height + 27 }, { 32, 16, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(21725), { 0, 16, height }, { 16, 16, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21745), { 0, 16, height },
                        { { 0, 16, height + 27 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21730), { 16, 16, height }, { 16, 16, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21750), { 16, 16, height },
                        { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21735), { 16, 0, height }, { 16, 16, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21755), { 16, 0, height },
                        { { 16, 0, height + 27 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21720), { 0, 0, height }, { 16, 16, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21740), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(21724), { 16, 0, height }, { 16, 34, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21744), { 16, 0, height },
                        { { 16, 0, height + 27 }, { 16, 34, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21729), { 0, 0, height }, { 16, 32, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21749), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21734), { 0, 0, height }, { 16, 32, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21754), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21719), { 16, 0, height }, { 16, 32, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21739), { 16, 0, height },
                        { { 16, 0, height + 27 }, { 16, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(21723), { 2, 0, height }, { 32, 32, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21743), { 2, 0, height },
                        { { 2, 0, height + 27 }, { 32, 32, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21728), { 2, 0, height }, { 27, 32, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21748), { 2, 0, height },
                        { { 2, 0, height + 27 }, { 27, 32, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21733), { 2, 0, height }, { 27, 32, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21753), { 2, 0, height },
                        { { 2, 0, height + 27 }, { 27, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21718), { 2, 0, height }, { 32, 32, 2 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21738), { 2, 0, height },
                        { { 2, 0, height + 27 }, { 32, 32, 0 } });
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

/** rct2: 0x0077847C */
static void SideFrictionRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    SideFrictionRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0077848C */
static void SideFrictionRCTrackSBendLeft(
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
                        session, direction, session.TrackColours.WithIndex(21888), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21904), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21892), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21908), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21891), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21907), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21895), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21911), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(21889), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21905), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 26, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21893), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21909), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 26, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21890), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21906), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 26, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21894), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21910), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 26, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
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
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21890), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21906), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 26, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21894), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21910), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 26, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21889), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21905), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 26, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21893), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21909), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 26, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(21891), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21907), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21895), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21911), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21888), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21904), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21892), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21908), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x0077849C */
static void SideFrictionRCTrackSBendRight(
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
                        session, direction, session.TrackColours.WithIndex(21896), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21912), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21900), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21916), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21899), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21915), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21903), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21919), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(21897), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21913), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 26, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21901), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21917), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 26, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21898), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21914), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 26, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21902), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21918), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 26, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(21898), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21914), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 26, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21902), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21918), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 26, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21897), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21913), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 26, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21901), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21917), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 26, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::SBendRight>(
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
                        session, direction, session.TrackColours.WithIndex(21899), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21915), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21903), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21919), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21896), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21912), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21900), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21916), { 0, 0, height },
                        { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x007784DC */
static void SideFrictionRCTrackLeftQuarterTurn3(
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
                        session, direction, session.TrackColours.WithIndex(21699), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21711), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21702), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21714), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21705), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21717), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21696), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21708), { 0, 0, height },
                        { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn3Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21698), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21710), { 0, 0, height },
                        { { 16, 0, height + 27 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21701), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21713), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21704), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21716), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21695), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21707), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(21697), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21709), { 0, 0, height },
                        { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21700), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21712), { 0, 0, height },
                        { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21703), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21715), { 0, 0, height },
                        { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21694), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21706), { 0, 0, height },
                        { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn3Tiles>(
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

/** rct2: 0x007784EC */
static void SideFrictionRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    SideFrictionRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007784FC */
static void SideFrictionRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(21610), { 0, 0, height },
                { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(21612), { 0, 0, height },
                { { 0, 2, height + 27 }, { 32, 27, 0 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(21611), { 0, 0, height },
                { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(21613), { 0, 0, height },
                { { 0, 2, height + 27 }, { 32, 27, 0 } });
            break;
    }
    DrawSupportForSequenceA<TrackElemType::Brakes>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007785DC */
static void SideFrictionRCTrackLeftEighthToDiag(
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
                        session, direction, session.TrackColours.WithIndex(21790), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21806), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21794), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21810), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21798), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21814), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21802), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21818), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftEighthToDiag>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(21791), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21807), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21795), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21811), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21799), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21815), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21803), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21819), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftEighthToDiag>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21792), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21808), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21796), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21812), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21800), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21816), { 0, 0, height },
                        { { 16, 0, height + 27 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21804), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21820), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::LeftEighthToDiag>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            DrawSupportForSequenceA<TrackElemType::LeftEighthToDiag>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21793), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21809), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21797), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21813), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21801), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21817), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21805), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21821), { 0, 0, height },
                        { { 16, 0, height + 27 }, { 16, 16, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007785EC */
static void SideFrictionRCTrackRightEighthToDiag(
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
                        session, direction, session.TrackColours.WithIndex(21758), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21774), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21762), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21778), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21766), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21782), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21770), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21786), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::RightEighthToDiag>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(21759), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21775), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21763), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21779), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21767), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21783), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21771), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21787), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::RightEighthToDiag>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21760), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21776), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21764), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21780), { 0, 0, height },
                        { { 16, 0, height + 27 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21768), { 0, 0, height },
                        { { 4, 4, height }, { 28, 28, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21784), { 0, 0, height },
                        { { 4, 4, height + 27 }, { 28, 28, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21772), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21788), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 16, 0 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::RightEighthToDiag>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            DrawSupportForSequenceA<TrackElemType::RightEighthToDiag>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21761), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21777), { 0, 0, height },
                        { { 16, 0, height + 27 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21765), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21781), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21769), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21785), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21773), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21789), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007785FC */
static void SideFrictionRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    SideFrictionRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0077860C */
static void SideFrictionRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    SideFrictionRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x0077850C */
static void SideFrictionRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    constexpr ImageIndex images[2][kNumOrthogonalDirections] = {
        { 21822, 21823, 21824, 21825 },
        { 21846, 21847, 21848, 21849 },
    };
    const ImageIndex additionalImages[2][kNumOrthogonalDirections] = {
        { 21826, kImageIndexUndefined, 21827, kImageIndexUndefined },
        { 21850, kImageIndexUndefined, 21851, kImageIndexUndefined },
    };

    TrackPaintUtilDiagTilesPaint(
        session, 2, height, direction, trackSequence, images[trackElement.HasChain()], defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    switch (trackSequence)
    {
        case 1:
            if (direction == 0)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(additionalImages[trackElement.HasChain()][direction]),
                    { -16, -16, height }, { { -16, -16, height + 27 }, { 32, 32, 0 } });
            }
            break;
        case 2:
            if (direction == 2)
            {
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(additionalImages[trackElement.HasChain()][direction]),
                    { -16, -16, height }, { { -16, -16, height + 27 }, { 32, 32, 0 } });
            }
            break;
    }
    DrawSupportForSequenceA<TrackElemType::DiagFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0077853C */
static void SideFrictionRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21867), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21843), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21864), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21868), { -16, -16, height },
                            { { -16, -16, height + 43 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21840), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21844), { -16, -16, height },
                            { { -16, -16, height + 43 }, { 32, 32, 0 } });
                        break;
                }
            }
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21866), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21869), { -16, -16, height },
                            { { -16, -16, height + 43 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21842), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21845), { -16, -16, height },
                            { { -16, -16, height + 43 }, { 32, 32, 0 } });
                        break;
                }
            }
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21865), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21841), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            break;
    }
    DrawSupportForSequenceB<TrackElemType::DiagUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0077851C */
static void SideFrictionRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21855), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21831), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21852), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21856), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21828), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21832), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            DrawSupportForSequenceA<TrackElemType::DiagFlatToUp25>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21854), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21857), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21830), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21833), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            DrawSupportForSequenceA<TrackElemType::DiagFlatToUp25>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21853), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21829), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x0077852C */
static void SideFrictionRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21861), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21837), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21858), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21862), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21834), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21838), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21860), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21863), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21836), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21839), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21859), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21835), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }

            break;
    }
    DrawSupportForSequenceB<TrackElemType::DiagUp25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0077859C */
static void SideFrictionRCTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21865), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21841), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21866), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21869), { -16, -16, height },
                            { { -16, -16, height + 43 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21842), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21845), { -16, -16, height },
                            { { -16, -16, height + 43 }, { 32, 32, 0 } });
                        break;
                }
            }
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21864), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21868), { -16, -16, height },
                            { { -16, -16, height + 43 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21840), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21844), { -16, -16, height },
                            { { -16, -16, height + 43 }, { 32, 32, 0 } });
                        break;
                }
            }
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21867), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21843), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }

            break;
    }
    DrawSupportForSequenceB<TrackElemType::DiagDown25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0077857C */
static void SideFrictionRCTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21859), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21835), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21860), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21863), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21836), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21839), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21858), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21862), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21834), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21838), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21861), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21837), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            break;
    }

    DrawSupportForSequenceB<TrackElemType::DiagFlatToDown25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0077858C */
static void SideFrictionRCTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21853), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21829), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21854), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21857), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21830), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21833), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            DrawSupportForSequenceA<TrackElemType::DiagDown25ToFlat>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21852), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21856), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21828), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21832), { -16, -16, height },
                            { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
            }
            DrawSupportForSequenceA<TrackElemType::DiagDown25ToFlat>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21855), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(21831), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 2 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void SideFrictionRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_DIR_0_A), { 0, 0, height },
                { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_DIR_0_B), { 0, 0, height },
                { { 0, 26, height + 5 }, { 32, 1, 9 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_DIR_1_A), { 0, 0, height },
                { { 0, 2, height }, { 32, 27, 2 } });

            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_DIR_1_B), { 0, 0, height },
                { { 0, 26, height + 5 }, { 32, 1, 9 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_DIR_2_A), { 0, 0, height },
                { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_DIR_2_B), { 0, 0, height },
                { { 0, 26, height + 5 }, { 32, 1, 9 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_DIR_3_A), { 0, 0, height },
                { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_DIR_3_B), { 0, 0, height },
                { { 0, 26, height + 5 }, { 32, 1, 9 } });
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

static void SideFrictionRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SideFrictionRCTrack60DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static void SideFrictionRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_0_A),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_0_B),
                { 0, 0, height }, { { 0, 26, height + 5 }, { 32, 1, 9 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_1_A),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_1_B),
                { 0, 0, height }, { { 0, 26, height + 5 }, { 32, 1, 40 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_2_A),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_2_B),
                { 0, 0, height }, { { 0, 26, height + 5 }, { 32, 1, 40 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_3_A),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_25_DEG_UP_TO_60_DEG_UP_DIR_3_B),
                { 0, 0, height }, { { 0, 26, height + 5 }, { 32, 1, 9 } });
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

static void SideFrictionRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SideFrictionRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static void SideFrictionRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_0_A),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_0_B),
                { 0, 0, height }, { { 0, 26, height + 5 }, { 32, 1, 9 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_1_A),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_1_B),
                { 0, 0, height }, { { 0, 26, height + 5 }, { 32, 1, 36 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_2_A),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_2_B),
                { 0, 0, height }, { { 0, 26, height + 5 }, { 32, 1, 36 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_3_A),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_60_DEG_UP_TO_25_DEG_UP_DIR_3_B),
                { 0, 0, height }, { { 0, 26, height + 5 }, { 32, 1, 9 } });
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

static void SideFrictionRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SideFrictionRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static void SideFrictionRCTrackDiag60DegUp(
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
                        session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_3_A),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_0_A),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_0_B),
                        { -16, -16, height }, { { -16, -16, height + 43 }, { 32, 32, 0 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_2_A),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_2_B),
                        { -16, -16, height }, { { -16, -16, height + 43 }, { 32, 32, 0 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_DIR_1_A),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }

            break;
    }
    DrawSupportForSequenceB<TrackElemType::DiagUp60>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void SideFrictionRCTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SideFrictionRCTrackDiag60DegUp(session, ride, 3 - trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static void SideFrictionRCTrackDiag60DegUpTo25DegUp(
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
                        session, direction,
                        session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_3_A),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_0_A),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_0_B),
                        { -16, -16, height }, { { -16, -16, height + 59 }, { 32, 32, 0 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_2_A),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_2_B),
                        { -16, -16, height }, { { -16, -16, height + 59 }, { 32, 32, 0 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_60_DEG_UP_TO_25_DEG_UP_DIR_1_A),
                        { -16, -16, height }, { { 0, 0, height }, { 16, 16, 2 } });
                    break;
            }

            break;
    }
    DrawSupportForSequenceB<TrackElemType::DiagUp60ToUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void SideFrictionRCTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SideFrictionRCTrackDiag60DegUpTo25DegUp(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
};

static void SideFrictionRCTrackDiag25DegUpTo60DegUp(
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
                        session, direction,
                        session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_25_DEG_UP_TO_60_DEG_UP_DIR_3_A),
                        { -16 + 4, -16 + 2, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_25_DEG_UP_TO_60_DEG_UP_DIR_0_A),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(24033), { -16, -16, height },
                        { { -16, -16, height + 59 }, { 32, 32, 0 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_25_DEG_UP_TO_60_DEG_UP_DIR_2_A),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(24034), { -16, -16, height },
                        { { -16, -16, height + 59 }, { 32, 32, 0 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_SIDE_FRICTION_DIAG_25_DEG_UP_TO_60_DEG_UP_DIR_1_A),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }

            break;
    }
    DrawSupportForSequenceB<TrackElemType::DiagUp25ToUp60>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void SideFrictionRCTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SideFrictionRCTrackDiag25DegUpTo60DegUp(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
};

TrackPaintFunction GetTrackPaintFunctionSideFrictionRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return SideFrictionRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return SideFrictionRCTrackStation;
        case TrackElemType::Up25:
            return SideFrictionRCTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return SideFrictionRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return SideFrictionRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return SideFrictionRCTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return SideFrictionRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return SideFrictionRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return SideFrictionRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return SideFrictionRCTrackRightQuarterTurn5;
        case TrackElemType::SBendLeft:
            return SideFrictionRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return SideFrictionRCTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return SideFrictionRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return SideFrictionRCTrackRightQuarterTurn3;
        case TrackElemType::Brakes:
            return SideFrictionRCTrackBrakes;
        case TrackElemType::LeftEighthToDiag:
            return SideFrictionRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return SideFrictionRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return SideFrictionRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return SideFrictionRCTrackRightEighthToOrthogonal;
        case TrackElemType::DiagFlat:
            return SideFrictionRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return SideFrictionRCTrackDiag25DegUp;
        case TrackElemType::DiagFlatToUp25:
            return SideFrictionRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return SideFrictionRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return SideFrictionRCTrackDiag25DegDown;
        case TrackElemType::DiagFlatToDown25:
            return SideFrictionRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return SideFrictionRCTrackDiag25DegDownToFlat;

        // Added by OpenRCT2
        case TrackElemType::Down25ToDown60:
            return SideFrictionRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return SideFrictionRCTrack60DegDownTo25DegDown;
        case TrackElemType::Up25ToUp60:
            return SideFrictionRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return SideFrictionRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up60:
            return SideFrictionRCTrack60DegUp;
        case TrackElemType::Down60:
            return SideFrictionRCTrack60DegDown;

        case TrackElemType::DiagUp60:
            return SideFrictionRCTrackDiag60DegUp;
        case TrackElemType::DiagDown60:
            return SideFrictionRCTrackDiag60DegDown;
        case TrackElemType::DiagUp60ToUp25:
            return SideFrictionRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagDown25ToDown60:
            return SideFrictionRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagUp25ToUp60:
            return SideFrictionRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagDown60ToDown25:
            return SideFrictionRCTrackDiag60DegDownTo25DegDown;
        default:
            return TrackPaintFunctionDummy;
    }
}
