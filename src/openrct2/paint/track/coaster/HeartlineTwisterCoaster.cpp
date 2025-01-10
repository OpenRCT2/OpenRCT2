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

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

/** rct2: 0x0087694C */
static void HeartlineTwisterRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21354), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21356), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 26 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21355), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21357), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 26 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21358), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21360), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 26 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21359), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21361), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 26 } });
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
                    session, direction, session.TrackColours.WithIndex(21294), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21296), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 26 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21295), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21297), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 26 } });
                break;
        }
    }

    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void HeartlineTwisterRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][3] = {
        { 19732, SPR_STATION_BASE_B_SW_NE },
        { 19733, SPR_STATION_BASE_B_NW_SE },
        { 19732, SPR_STATION_BASE_B_SW_NE },
        { 19733, SPR_STATION_BASE_B_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(imageIds[direction][0]), { 0, 0, height },
        { { 0, 6, height + 3 }, { 32, 20, 1 } });
    PaintAddImageAsParentRotated(
        session, direction, GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][1]), { 0, 0, height },
        { 32, 32, 1 });
    DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Tubes);
    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0087695C */
static void HeartlineTwisterRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21378), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21382), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 50 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21379), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21383), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 50 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21380), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21384), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 50 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21381), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21385), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 50 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21322), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21326), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 50 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21323), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21327), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 50 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21324), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21328), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 50 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21325), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21329), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 50 } });
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

/** rct2: 0x008769FC */
static void HeartlineTwisterRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21402), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21406), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
            case 1:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21403), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21407), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
            case 2:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21404), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21408), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21405), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21409), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21346), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21350), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
            case 1:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21347), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21351), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
            case 2:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21348), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21352), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21349), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21353), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
        }
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

/** rct2: 0x0087696C */
static void HeartlineTwisterRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21362), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21366), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 42 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21363), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21367), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 42 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21364), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21368), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 42 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21365), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21369), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 42 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21306), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21310), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 42 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21307), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21311), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 42 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21308), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21312), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 42 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21309), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21313), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 42 } });
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

/** rct2: 0x008769BC */
static void HeartlineTwisterRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21386), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21390), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 1:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21387), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21391), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 2:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21388), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21392), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21389), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21393), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21330), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21334), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 1:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21331), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21335), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 2:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21332), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21336), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21333), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21337), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
        }
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

/** rct2: 0x008769CC */
static void HeartlineTwisterRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21394), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21398), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 1:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21395), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21399), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 2:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21396), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21400), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21397), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21401), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21338), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21342), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 1:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21339), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21343), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 2:
                session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21340), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21344), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21341), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21345), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
        }
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

/** rct2: 0x0087697C */
static void HeartlineTwisterRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21370), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21374), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 34 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21371), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21375), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 34 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21372), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21376), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 34 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21373), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21377), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 34 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21314), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21318), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 34 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21315), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21319), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 34 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21316), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21320), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 34 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21317), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21321), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 34 } });
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

/** rct2: 0x0087698C */
static void HeartlineTwisterRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    HeartlineTwisterRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x00876A0C */
static void HeartlineTwisterRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    HeartlineTwisterRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0087699C */
static void HeartlineTwisterRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    HeartlineTwisterRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008769DC */
static void HeartlineTwisterRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    HeartlineTwisterRCTrack60DegUpTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008769EC */
static void HeartlineTwisterRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    HeartlineTwisterRCTrack25DegUpTo60DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008769AC */
static void HeartlineTwisterRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    HeartlineTwisterRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x00876A6C */
static void HeartlineTwisterRCTrackHeartlineTransferUp(
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
                        session, direction, session.TrackColours.WithIndex(21306), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21310), { 0, 0, height },
                        { { 0, 6, height + 23 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21307), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21311), { 0, 0, height },
                        { { 0, 6, height + 23 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21308), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21312), { 0, 0, height },
                        { { 0, 6, height + 23 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21309), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21313), { 0, 0, height },
                        { { 0, 6, height + 23 }, { 32, 20, 0 } });
                    break;
            }

            DrawSupportForSequenceA<TrackElemType::HeartLineTransferUp>(
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
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21314), { 0, 0, height + 8 },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21308), { 0, 0, height + 16 },
                        { { 0, 6, height + 16 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21318), { 0, 0, height + 8 },
                        { { 0, 27, height + 8 }, { 32, 1, 34 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21312), { 0, 0, height + 16 },
                        { { 0, 27, height + 16 }, { 32, 1, 34 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21315), { 0, 0, height + 8 },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21309), { 0, 0, height + 16 },
                        { { 0, 6, height + 16 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21319), { 0, 0, height + 8 },
                        { { 0, 27, height + 8 }, { 32, 1, 34 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21313), { 0, 0, height + 16 },
                        { { 0, 27, height + 16 }, { 32, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21316), { 0, 0, height + 8 },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21306), { 0, 0, height + 16 },
                        { { 0, 6, height + 16 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21320), { 0, 0, height + 8 },
                        { { 0, 27, height + 8 }, { 32, 1, 34 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21310), { 0, 0, height + 16 },
                        { { 0, 27, height + 16 }, { 32, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21317), { 0, 0, height + 8 },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21307), { 0, 0, height + 16 },
                        { { 0, 6, height + 16 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21321), { 0, 0, height + 8 },
                        { { 0, 27, height + 8 }, { 32, 1, 34 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21311), { 0, 0, height + 16 },
                        { { 0, 27, height + 16 }, { 32, 1, 34 } });
                    break;
            }

            DrawSupportForSequenceA<TrackElemType::HeartLineTransferUp>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21298), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21302), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21299), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21303), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21300), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21304), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21301), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21305), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }

            DrawSupportForSequenceA<TrackElemType::HeartLineTransferUp>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21316), { 0, 0, height - 8 },
                        { { 0, 6, height - 7 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21320), { 0, 0, height - 8 },
                        { { 0, 27, height - 8 }, { 32, 1, 34 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21317), { 0, 0, height - 8 },
                        { { 0, 6, height - 7 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21321), { 0, 0, height - 8 },
                        { { 0, 27, height - 8 }, { 32, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21314), { 0, 0, height - 8 },
                        { { 0, 6, height - 7 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21318), { 0, 0, height - 8 },
                        { { 0, 27, height - 8 }, { 32, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21315), { 0, 0, height - 8 },
                        { { 0, 6, height - 7 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21319), { 0, 0, height - 8 },
                        { { 0, 27, height - 8 }, { 32, 1, 34 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            else
            {
                PaintUtilPushTunnelRotated(session, direction, height - 16, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x00876A7C */
static void HeartlineTwisterRCTrackHeartlineTransferDown(
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
                        session, direction, session.TrackColours.WithIndex(21316), { 0, 0, height - 8 },
                        { { 0, 6, height - 7 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21320), { 0, 0, height - 8 },
                        { { 0, 27, height - 8 }, { 32, 1, 34 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21317), { 0, 0, height - 8 },
                        { { 0, 6, height - 7 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21321), { 0, 0, height - 8 },
                        { { 0, 27, height - 8 }, { 32, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21314), { 0, 0, height - 8 },
                        { { 0, 6, height - 7 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21318), { 0, 0, height - 8 },
                        { { 0, 27, height - 8 }, { 32, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21315), { 0, 0, height - 8 },
                        { { 0, 6, height - 7 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21319), { 0, 0, height - 8 },
                        { { 0, 27, height - 8 }, { 32, 1, 34 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            else
            {
                PaintUtilPushTunnelRotated(session, direction, height - 16, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21314), { 0, 0, height + 8 },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21308), { 0, 0, height + 16 },
                        { { 0, 6, height + 16 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21318), { 0, 0, height + 8 },
                        { { 0, 27, height + 8 }, { 32, 1, 34 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21312), { 0, 0, height + 16 },
                        { { 0, 27, height + 16 }, { 32, 1, 34 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21315), { 0, 0, height + 8 },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21309), { 0, 0, height + 16 },
                        { { 0, 6, height + 16 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21319), { 0, 0, height + 8 },
                        { { 0, 27, height + 8 }, { 32, 1, 34 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21313), { 0, 0, height + 16 },
                        { { 0, 27, height + 16 }, { 32, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21316), { 0, 0, height + 8 },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21306), { 0, 0, height + 16 },
                        { { 0, 6, height + 16 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21320), { 0, 0, height + 8 },
                        { { 0, 27, height + 8 }, { 32, 1, 34 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21310), { 0, 0, height + 16 },
                        { { 0, 27, height + 16 }, { 32, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21317), { 0, 0, height + 8 },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21307), { 0, 0, height + 16 },
                        { { 0, 6, height + 16 }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21321), { 0, 0, height + 8 },
                        { { 0, 27, height + 8 }, { 32, 1, 34 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex(21311), { 0, 0, height + 16 },
                        { { 0, 27, height + 16 }, { 32, 1, 34 } });
                    break;
            }

            DrawSupportForSequenceA<TrackElemType::HeartLineTransferDown>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21298), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21302), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21299), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21303), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21300), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21304), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21301), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21305), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }

            DrawSupportForSequenceA<TrackElemType::HeartLineTransferDown>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21306), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21310), { 0, 0, height },
                        { { 0, 6, height + 23 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21307), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21311), { 0, 0, height },
                        { { 0, 6, height + 23 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21308), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21312), { 0, 0, height },
                        { { 0, 6, height + 23 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21309), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21313), { 0, 0, height },
                        { { 0, 6, height + 23 }, { 32, 20, 0 } });
                    break;
            }

            DrawSupportForSequenceA<TrackElemType::HeartLineTransferDown>(
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
            break;
    }
}

/** rct2: 0x00876A4C */
static void HeartlineTwisterRCTrackLeftHeartlineRoll(
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
                        session, direction, session.TrackColours.WithIndex(21410), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21422), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21413), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21425), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21416), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21428), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21419), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21431), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21411), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21423), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21414), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21426), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21417), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21429), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21420), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21432), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21412), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21424), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21415), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21427), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21418), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21430), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21421), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21433), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21418), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21430), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21421), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21433), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21412), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21424), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21415), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21427), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21417), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21429), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21420), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21432), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21411), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21423), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21414), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21426), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21416), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21428), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21419), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21431), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21410), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21422), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21413), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21425), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<TrackElemType::LeftHeartLineRoll>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00876A5C */
static void HeartlineTwisterRCTrackRightHeartlineRoll(
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
                        session, direction, session.TrackColours.WithIndex(21434), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21446), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21437), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21449), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21440), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21452), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21443), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21455), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21435), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21447), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21438), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21450), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21441), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21453), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21444), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21456), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21436), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21448), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21439), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21451), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21442), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21454), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21445), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21457), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21442), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21454), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21445), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21457), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21436), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21448), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21439), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21451), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21441), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21453), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21444), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21456), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21435), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21447), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21438), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21450), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21440), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21452), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21443), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21455), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21434), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21446), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21437), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21449), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<TrackElemType::RightHeartLineRoll>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

TrackPaintFunction GetTrackPaintFunctionHeartlineTwisterRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return HeartlineTwisterRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return HeartlineTwisterRCTrackStation;
        case TrackElemType::Up25:
            return HeartlineTwisterRCTrack25DegUp;
        case TrackElemType::Up60:
            return HeartlineTwisterRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return HeartlineTwisterRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return HeartlineTwisterRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return HeartlineTwisterRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return HeartlineTwisterRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return HeartlineTwisterRCTrack25DegDown;
        case TrackElemType::Down60:
            return HeartlineTwisterRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return HeartlineTwisterRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return HeartlineTwisterRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return HeartlineTwisterRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return HeartlineTwisterRCTrack25DegDownToFlat;
        case TrackElemType::HeartLineTransferUp:
            return HeartlineTwisterRCTrackHeartlineTransferUp;
        case TrackElemType::HeartLineTransferDown:
            return HeartlineTwisterRCTrackHeartlineTransferDown;
        case TrackElemType::LeftHeartLineRoll:
            return HeartlineTwisterRCTrackLeftHeartlineRoll;
        case TrackElemType::RightHeartLineRoll:
            return HeartlineTwisterRCTrackRightHeartlineRoll;
        default:
            return TrackPaintFunctionDummy;
    }
}
