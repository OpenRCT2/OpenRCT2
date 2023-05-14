/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.TileElement.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"

// Closed block brakes and regular brakes use the same images
static constexpr const uint32_t T_REX_COASTER_BRAKE_SW_NE_OPEN_1 = 18076;
static constexpr const uint32_t T_REX_COASTER_BRAKE_NW_SE_OPEN_1 = 18077;
static constexpr const uint32_t T_REX_COASTER_BRAKE_SW_NE_CLOSED_1 = 18078;
static constexpr const uint32_t T_REX_COASTER_BRAKE_NW_SE_CLOSED_1 = 18079;
static constexpr const uint32_t T_REX_COASTER_BRAKE_SW_NE_OPEN_2 = 18080;
static constexpr const uint32_t T_REX_COASTER_BRAKE_NW_SE_OPEN_2 = 18081;
static constexpr const uint32_t T_REX_COASTER_BRAKE_SW_NE_CLOSED_2 = 18082;
static constexpr const uint32_t T_REX_COASTER_BRAKE_NW_SE_CLOSED_2 = 18083;

static constexpr const uint32_t _T_REXCoasterBrakeImages[NumOrthogonalDirections][2][2] = {
    { { T_REX_COASTER_BRAKE_SW_NE_OPEN_1, T_REX_COASTER_BRAKE_SW_NE_OPEN_2 },
      { T_REX_COASTER_BRAKE_SW_NE_CLOSED_1, T_REX_COASTER_BRAKE_SW_NE_CLOSED_2 } },
    { { T_REX_COASTER_BRAKE_NW_SE_OPEN_1, T_REX_COASTER_BRAKE_NW_SE_OPEN_2 },
      { T_REX_COASTER_BRAKE_NW_SE_CLOSED_1, T_REX_COASTER_BRAKE_NW_SE_CLOSED_2 } },
    { { T_REX_COASTER_BRAKE_SW_NE_OPEN_1, T_REX_COASTER_BRAKE_SW_NE_OPEN_2 },
      { T_REX_COASTER_BRAKE_SW_NE_CLOSED_1, T_REX_COASTER_BRAKE_SW_NE_CLOSED_2 } },
    { { T_REX_COASTER_BRAKE_NW_SE_OPEN_1, T_REX_COASTER_BRAKE_NW_SE_OPEN_2 },
      { T_REX_COASTER_BRAKE_NW_SE_CLOSED_1, T_REX_COASTER_BRAKE_NW_SE_CLOSED_2 } },
};

/** rct2: 0x008AD674 */
static void T_REXRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasCableLift())
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18692), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18693), { 0, 0, height },
                    { { 6, 0, height }, { 20, 32, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18382), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18383), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18384), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18385), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18074), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18075), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void T_REXRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { 18084, SPR_STATION_BASE_A_SW_NE },
        { 18085, SPR_STATION_BASE_A_NW_SE },
        { 18084, SPR_STATION_BASE_A_SW_NE },
        { 18085, SPR_STATION_BASE_A_NW_SE },
    };

    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        bool isClosed = trackElement.IsBrakeClosed();
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_T_REXCoasterBrakeImages[direction][isClosed][0]),
            { 0, 0, height }, { { 0, 6, height + 3 }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]), { 0, 0, height },
            { { 0, 6, height + 3 }, { 32, 20, 1 } });
    }
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][1]), { 0, 0, height },
        { 32, 32, 1 });
    TrackPaintUtilDrawStationMetalSupports2(
        session, direction, height, session.TrackColours[SCHEME_SUPPORTS], MetalSupportType::Tubes);
    TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, 9, 11);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008AD684 */
static void T_REXRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasCableLift())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18702), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18703), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18704), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18705), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18394), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18395), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18396), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18397), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18134), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18135), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18136), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18137), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008AD694 */
static void T_REXRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasCableLift())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18718), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18719), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18720), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18721), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18150), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18151), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18152), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 98 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18153), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008AD6A4 */
static void T_REXRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasCableLift())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18694), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18695), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18696), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18697), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18386), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18387), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18388), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18389), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18126), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18127), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18128), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18129), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AD6B4 */
static void T_REXRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasCableLift())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18706), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18707), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18710), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18708), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18711), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18709), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18138), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18139), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18142), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18140), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18143), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18141), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008AD6C4 */
static void T_REXRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasCableLift())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18712), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18713), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18716), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18714), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18717), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18715), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18144), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18145), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18148), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18146), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18149), { 0, 0, height },
                    { { 0, 27, height }, { 32, 1, 66 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18147), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008AD6D4 */
static void T_REXRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasCableLift())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18698), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18699), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18700), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18701), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18390), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18391), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18392), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18393), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18130), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18131), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18132), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18133), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008AD6E4 */
static void T_REXRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD6F4 */
static void T_REXRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD704 */
static void T_REXRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD714 */
static void T_REXRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD724 */
static void T_REXRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD734 */
static void T_REXRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD744 */
static void T_REXRCTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18189), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18194), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18199), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18184), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18188), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18193), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18198), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18183), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18187), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18192), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18197), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18182), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18186), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18191), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18196), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18181), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18185), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18190), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18195), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18180), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD754 */
static void T_REXRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    T_REXRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AD764 */
static void T_REXRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18086), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18094), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18087), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18095), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18088), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18089), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008AD774 */
static void T_REXRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18090), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18091), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18092), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18096), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18093), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18097), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008AD784 */
static void T_REXRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18092), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18096), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18093), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18097), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18090), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18091), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008AD794 */
static void T_REXRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18088), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18089), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18086), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18094), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18087), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18095), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008AD7A4 */
static void T_REXRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18209), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18220), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18214), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18219), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18204), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18208), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18213), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18218), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18203), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18207), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18212), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18217), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18202), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18206), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18211), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18216), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18201), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18205), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18210), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18215), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18221), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18200), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD7B4 */
static void T_REXRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    T_REXRCTrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AD7C4 */
static void T_REXRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18098), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18102), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18099), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18103), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18100), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18101), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AD7D4 */
static void T_REXRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18104), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18105), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18106), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18108), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18107), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18109), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AD7E4 */
static void T_REXRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18110), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18114), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18111), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18115), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18112), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18113), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008AD7F4 */
static void T_REXRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18116), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18117), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18118), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18120), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18119), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18121), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008AD804 */
static void T_REXRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD814 */
static void T_REXRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD824 */
static void T_REXRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD834 */
static void T_REXRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD844 */
static void T_REXRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18122), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18123), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18124), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18125), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008AD854 */
static void T_REXRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AD864 */
static void T_REXRCTrackLeftQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18274), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18279), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18284), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18289), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18275), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18280), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18285), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18290), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18276), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18281), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18286), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18291), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18277), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18282), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18287), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18292), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18278), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18283), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18288), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18293), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AD874 */
static void T_REXRCTrackRightQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18254), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18259), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18264), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18269), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18255), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18260), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18265), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18270), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18256), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18261), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18266), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18271), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18257), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18262), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18267), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18272), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18258), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18263), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18268), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18273), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AD884 */
static void T_REXRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    T_REXRCTrackRightQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008AD894 */
static void T_REXRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    T_REXRCTrackLeftQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AD8A4 */
static void T_REXRCTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18238), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18242), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18241), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18245), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18239), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18243), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18240), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18244), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18240), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18244), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18239), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18243), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18241), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18245), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18238), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18242), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD8B4 */
static void T_REXRCTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18246), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18250), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18249), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18253), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18247), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18251), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18248), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18252), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18248), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18252), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18247), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18251), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18249), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18253), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18246), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18250), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD8F4 */
static void T_REXRCTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18159), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18162), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18165), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18156), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18158), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18161), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18164), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18155), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18157), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18160), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18163), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18154), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD904 */
static void T_REXRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    T_REXRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AD914 */
static void T_REXRCTrackLeftQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18171), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18178), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18174), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18177), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18168), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18170), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18173), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18176), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18167), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18169), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18172), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18175), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18179), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18166), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD924 */
static void T_REXRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    T_REXRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AD934 */
static void T_REXRCTrackLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18305), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18307), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18309), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18303), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18304), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18306), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18308), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18302), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AD944 */
static void T_REXRCTrackRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18294), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18296), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18298), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18300), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18295), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18297), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18299), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18301), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AD954 */
static void T_REXRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    T_REXRCTrackRightQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008AD964 */
static void T_REXRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    T_REXRCTrackLeftQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AD974 */
static void T_REXRCTrackLeftHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18329), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18336), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18332), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18335), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18326), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18328), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18331), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18334), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18325), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18327), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18330), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18333), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18337), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18324), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18326), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18329), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18336), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18332), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18335), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18325), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18328), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18331), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18334), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18324), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18327), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18330), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18333), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18337), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD984 */
static void T_REXRCTrackRightHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18310), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18313), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18316), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18319), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18323), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18311), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18314), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18317), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18320), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18312), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18315), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18322), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18318), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18321), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18313), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18316), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18319), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18323), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18310), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18314), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18317), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18320), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18311), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18315), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18322), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18318), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18321), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18312), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD994 */
static void T_REXRCTrackLeftHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    T_REXRCTrackRightHalfBankedHelixUpSmall(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008AD9A4 */
static void T_REXRCTrackRightHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    T_REXRCTrackLeftHalfBankedHelixUpSmall(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AD9B4 */
static void T_REXRCTrackLeftHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18369), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18380), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18374), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18379), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18364), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18368), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18373), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18378), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18363), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18367), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18372), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18377), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18362), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18366), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18371), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18376), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18361), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18365), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18370), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18375), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18381), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18360), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18364), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18369), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18380), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18374), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18379), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 8:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18363), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18368), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18373), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18378), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 10:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18362), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18367), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18372), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18377), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 11:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18361), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18366), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18371), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18376), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 13:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18360), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18365), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18370), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18375), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18381), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD9C4 */
static void T_REXRCTrackRightHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18338), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18343), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18348), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18353), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18359), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18339), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18344), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18349), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18354), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18340), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18345), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18350), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18355), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18341), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18346), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18351), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18356), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18342), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18347), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18358), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18352), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18357), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18343), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18348), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18353), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18359), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18338), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 8:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18344), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18349), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18354), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18339), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 10:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18345), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18350), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18355), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18340), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 11:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18346), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18351), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18356), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18341), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 13:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18347), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18358), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18352), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18357), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18342), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD9D4 */
static void T_REXRCTrackLeftHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    T_REXRCTrackRightHalfBankedHelixUpLarge(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008AD9E4 */
static void T_REXRCTrackRightHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    T_REXRCTrackLeftHalfBankedHelixUpLarge(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008ADA14 */
static void T_REXRCTrackLeftQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18231), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18235), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18232), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18236), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18233), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18237), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18230), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18234), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
    }
    TrackPaintUtilLeftQuarterTurn1TileTunnel(session, direction, height, -8, TUNNEL_SQUARE_7, +56, TUNNEL_SQUARE_8);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008AD9F4 */
static void T_REXRCTrackRightQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18222), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18226), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18223), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18227), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18224), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18228), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18225), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18229), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
    }
    TrackPaintUtilRightQuarterTurn1TileTunnel(session, direction, height, -8, TUNNEL_SQUARE_7, +56, TUNNEL_SQUARE_8);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008ADA04 */
static void T_REXRCTrackLeftQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackRightQuarterTurn160DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008ADA24 */
static void T_REXRCTrackRightQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackLeftQuarterTurn160DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008ADA34 */
static void T_REXRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(T_REX_COASTER_BRAKE_SW_NE_CLOSED_1),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(T_REX_COASTER_BRAKE_SW_NE_CLOSED_2),
                { 0, 0, height }, { { 0, 27, height + 5 }, { 32, 1, 11 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(T_REX_COASTER_BRAKE_NW_SE_CLOSED_1),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(T_REX_COASTER_BRAKE_NW_SE_CLOSED_2),
                { 0, 0, height }, { { 0, 27, height + 5 }, { 32, 1, 11 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008ADC84 */
static void T_REXRCTrack25DegUpLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18560), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18561), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18562), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18563), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008ADC94 */
static void T_REXRCTrack25DegUpRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18564), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18565), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18566), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18567), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008ADA44 */
static void T_REXRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18074), { 0, 0, height },
                { { 0, 6, height + 3 }, { 32, 20, 0 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18075), { 0, 0, height },
                { { 0, 6, height + 3 }, { 32, 20, 0 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18074), { 0, 0, height },
                { { 0, 6, height + 3 }, { 32, 20, 0 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18075), { 0, 0, height },
                { { 0, 6, height + 3 }, { 32, 20, 0 } });
            break;
    }
    TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008ADCA4 */
static void T_REXRCTrack25DegDownLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack25DegUpRightBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADCB4 */
static void T_REXRCTrack25DegDownRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack25DegUpLeftBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADED4 */
static void T_REXRCTrackFlatTo60DegUpLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasCableLift())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18722), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18726), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18730), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18734), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18660), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18664), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18668), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18672), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            if (trackElement.HasCableLift())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18723), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18727), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18731), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18735), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18661), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18665), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18669), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18673), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            if (trackElement.HasCableLift())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18724), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18728), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18732), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18736), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18662), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18666), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18670), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18674), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            if (trackElement.HasCableLift())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18725), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18729), { 0, 0, height },
                            { { 0, 27, height }, { 32, 1, 98 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18733), { 0, 0, height },
                            { { 0, 27, height }, { 32, 1, 98 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18737), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18663), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18667), { 0, 0, height },
                            { { 0, 27, height }, { 32, 1, 98 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18671), { 0, 0, height },
                            { { 0, 27, height }, { 32, 1, 98 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18675), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 24, TUNNEL_SQUARE_8);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 24, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
    }
}

/** rct2: 0x008ADEE4 */
static void T_REXRCTrack60DegUpToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18676), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18680), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 98 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18684), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 98 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18688), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Tubes, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18677), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18681), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18685), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18689), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Tubes, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18678), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18682), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18686), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18690), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18679), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18683), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18687), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18691), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
            break;
    }
}

/** rct2: 0x008ADEF4 */
static void T_REXRCTrack60DegDownToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackFlatTo60DegUpLongBase(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADF04 */
static void T_REXRCTrackFlatTo60DegDownLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack60DegUpToFlatLongBase(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADF14 */
static void T_REXRCTrackCableLiftHill(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18698), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18699), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18700), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18701), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
            }
            else
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18700), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18701), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18698), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18699), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            else
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18714), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18717), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 66 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18715), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18712), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18713), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18716), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 66 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Tubes, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_SQUARE_8);
            }
            else
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18720), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 98 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18721), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18718), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18719), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 98 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Tubes, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_SQUARE_8);
            }
            else
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008ADA64 */
static void T_REXRCTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18414), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18418), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18422), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18426), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18415), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18419), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18423), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18427), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18416), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18420), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18424), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18428), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18417), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18421), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18425), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18429), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADA74 */
static void T_REXRCTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18398), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18402), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18406), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18410), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18399), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18403), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18407), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18411), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18400), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18404), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18408), { 0, 0, height },
                        { { 4, 4, height }, { 28, 28, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18412), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18401), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18405), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18409), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18413), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADA84 */
static void T_REXRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    T_REXRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADA94 */
static void T_REXRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    T_REXRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008ADAA4 */
static void T_REXRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18446), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18450), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18454), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18458), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18447), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18451), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 34, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18455), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18459), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18448), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18452), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18456), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18460), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18449), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18453), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 18, 0 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18457), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18461), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADAB4 */
static void T_REXRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18430), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18434), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18438), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18442), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18431), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18435), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18439), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 34, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18443), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18432), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18436), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18440), { 0, 0, height },
                        { { 4, 4, height + 27 }, { 28, 28, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18444), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18433), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18437), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18441), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 18, 0 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18445), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADAC4 */
static void T_REXRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    T_REXRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADAD4 */
static void T_REXRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    T_REXRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008ADA54 */
static void T_REXRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18527), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18465), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18524), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18462), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18526), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18464), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18525), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18463), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADB04 */
static void T_REXRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18539), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18477), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18536), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18474), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18538), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18476), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18537), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18475), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008ADB64 */
static void T_REXRCTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18489), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18486), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18488), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18487), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008ADAE4 */
static void T_REXRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18531), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18469), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18528), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18466), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18530), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18468), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18529), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18467), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008ADB44 */
static void T_REXRCTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18481), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18478), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18480), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18479), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADB54 */
static void T_REXRCTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18485), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18482), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18484), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18483), { -16, -16, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADAF4 */
static void T_REXRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18535), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18473), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18532), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18470), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18534), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18472), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18533), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18471), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008ADB34 */
static void T_REXRCTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18537), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18475), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18538), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18476), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18536), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18474), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18539), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18477), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008ADB94 */
static void T_REXRCTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18487), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18488), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18486), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18489), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008ADB14 */
static void T_REXRCTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18533), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18471), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18534), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18472), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18532), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18470), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18535), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18473), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008ADB74 */
static void T_REXRCTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18483), { -16, -16, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18484), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18482), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18485), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADB84 */
static void T_REXRCTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18479), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18480), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18478), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18481), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADB24 */
static void T_REXRCTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18529), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18467), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18530), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18468), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18528), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18466), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18531), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18469), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008ADBC4 */
static void T_REXRCTrackDiagFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18497), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18494), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18498), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18496), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18495), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADBD4 */
static void T_REXRCTrackDiagFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18502), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18499), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18501), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18503), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18500), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADBE4 */
static void T_REXRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18500), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18501), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18503), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18499), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18502), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADBF4 */
static void T_REXRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18495), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18496), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18494), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18498), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18497), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADC24 */
static void T_REXRCTrackDiagLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18517), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18514), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18518), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18516), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18515), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008ADC34 */
static void T_REXRCTrackDiagRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18522), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18519), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18521), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18523), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18520), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008ADC04 */
static void T_REXRCTrackDiag25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18507), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18504), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18508), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18506), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18505), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008ADC14 */
static void T_REXRCTrackDiag25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18512), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18509), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18511), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18513), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18510), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008ADC44 */
static void T_REXRCTrackDiagLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18510), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18511), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18513), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18509), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18512), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008ADC54 */
static void T_REXRCTrackDiagRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18505), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18506), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18504), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18508), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18507), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008ADC64 */
static void T_REXRCTrackDiag25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18520), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18521), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18523), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18519), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18522), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008ADC74 */
static void T_REXRCTrackDiag25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18515), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18516), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18514), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18518), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18517), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008ADBA4 */
static void T_REXRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18493), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18490), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18492), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18491), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADBB4 */
static void T_REXRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18491), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18492), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18490), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18493), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008ADEC4 */
static void T_REXRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isClosed = trackElement.IsBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_T_REXCoasterBrakeImages[direction][isClosed][0]),
        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_T_REXCoasterBrakeImages[direction][isClosed][1]),
        { 0, 0, height }, { { 0, 27, height + 5 }, { 32, 1, 11 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008ADCC4 */
static void T_REXRCTrackLeftBankedQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18655), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18657), { 0, 6, height },
                        { { 0, 27, height }, { 32, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18659), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18653), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18654), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18656), { 6, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18658), { 6, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18652), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADCD4 */
static void T_REXRCTrackRightBankedQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18644), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18646), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18648), { 0, 6, height },
                        { { 0, 27, height }, { 32, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18650), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18645), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18647), { 6, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18649), { 6, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18651), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADCE4 */
static void T_REXRCTrackLeftBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    T_REXRCTrackRightBankedQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008ADCF4 */
static void T_REXRCTrackRightBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    T_REXRCTrackLeftBankedQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008ADD04 */
static void T_REXRCTrackLeftBankedQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18624), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18629), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18634), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18639), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18625), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18630), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18635), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18640), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18626), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18631), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18636), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18641), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18627), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18632), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18637), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18642), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18628), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18633), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18638), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18643), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADD14 */
static void T_REXRCTrackRightBankedQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18604), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18609), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18614), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18619), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18605), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18610), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18615), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18620), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18606), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18611), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18616), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18621), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18607), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18612), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18617), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18622), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18608), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18613), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18618), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18623), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADD24 */
static void T_REXRCTrackLeftBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    T_REXRCTrackRightBankedQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008ADD34 */
static void T_REXRCTrackRightBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    T_REXRCTrackLeftBankedQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008ADD44 */
static void T_REXRCTrack25DegUpToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18568), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18569), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18576), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18570), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18571), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008ADD54 */
static void T_REXRCTrack25DegUpToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18572), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18573), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18574), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18577), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18575), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008ADD64 */
static void T_REXRCTrackLeftBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18578), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18579), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18586), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18580), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18581), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008ADD74 */
static void T_REXRCTrackRightBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18582), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18583), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18584), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18587), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18585), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008ADD84 */
static void T_REXRCTrack25DegDownToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackRightBanked25DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADD94 */
static void T_REXRCTrack25DegDownToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackLeftBanked25DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADDA4 */
static void T_REXRCTrackLeftBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack25DegUpToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADDB4 */
static void T_REXRCTrackRightBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrack25DegUpToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADDC4 */
static void T_REXRCTrackLeftBankedFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18588), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18589), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18590), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18591), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008ADDD4 */
static void T_REXRCTrackRightBankedFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18592), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18593), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18594), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18595), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008ADE04 */
static void T_REXRCTrackLeftBanked25DegUpToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18596), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18597), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18598), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18599), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008ADE14 */
static void T_REXRCTrackRightBanked25DegUpToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18600), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18601), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18602), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18603), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008ADE24 */
static void T_REXRCTrackLeftBankedFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackRightBanked25DegUpToRightBankedFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADE34 */
static void T_REXRCTrackRightBankedFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackLeftBanked25DegUpToLeftBankedFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADDE4 */
static void T_REXRCTrackLeftBanked25DegDownToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackRightBankedFlatToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADDF4 */
static void T_REXRCTrackRightBanked25DegDownToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackLeftBankedFlatToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADE44 */
static void T_REXRCTrackFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18540), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18541), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18548), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18542), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18543), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008ADE54 */
static void T_REXRCTrackFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18544), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18545), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18546), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18549), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18547), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008ADE64 */
static void T_REXRCTrackLeftBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18550), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18551), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18558), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18552), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18553), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008ADE74 */
static void T_REXRCTrackRightBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18554), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18555), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18556), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18559), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18557), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008ADE84 */
static void T_REXRCTrackFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackRightBanked25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADE94 */
static void T_REXRCTrackFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackLeftBanked25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADEA4 */
static void T_REXRCTrackLeftBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackFlatToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008ADEB4 */
static void T_REXRCTrackRightBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    T_REXRCTrackFlatToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void T_REXRCTrackBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    // These offsets could be moved to the g2.dat file when that supports offsets.
    int8_t ne_sw_offsetX = 7;
    int8_t ne_sw_offsetY = -15;
    int8_t nw_se_offsetX = -15;
    int8_t nw_se_offsetY = 7;

    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_T_REX_RC_BOOSTER_NE_SW),
                { ne_sw_offsetX, ne_sw_offsetY, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_T_REX_RC_BOOSTER_NW_SE),
                { nw_se_offsetX, nw_se_offsetY, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void T_REXRCTrackPoweredLift(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_T_REX_RC_POWERED_LIFT_0 + direction),
        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportType::Tubes, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionT_REXRC(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return T_REXRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return T_REXRCTrackStation;
        case TrackElemType::Up25:
            return T_REXRCTrack25DegUp;
        case TrackElemType::Up60:
            return T_REXRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return T_REXRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return T_REXRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return T_REXRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return T_REXRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return T_REXRCTrack25DegDown;
        case TrackElemType::Down60:
            return T_REXRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return T_REXRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return T_REXRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return T_REXRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return T_REXRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return T_REXRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return T_REXRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return T_REXRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return T_REXRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return T_REXRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return T_REXRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return T_REXRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return T_REXRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return T_REXRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return T_REXRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return T_REXRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return T_REXRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return T_REXRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return T_REXRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return T_REXRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return T_REXRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return T_REXRCTrackLeftBank;
        case TrackElemType::RightBank:
            return T_REXRCTrackRightBank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return T_REXRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return T_REXRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return T_REXRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return T_REXRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::SBendLeft:
            return T_REXRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return T_REXRCTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return T_REXRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return T_REXRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return T_REXRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return T_REXRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return T_REXRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return T_REXRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return T_REXRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return T_REXRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return T_REXRCTrackLeftHalfBankedHelixUpSmall;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return T_REXRCTrackRightHalfBankedHelixUpSmall;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return T_REXRCTrackLeftHalfBankedHelixDownSmall;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return T_REXRCTrackRightHalfBankedHelixDownSmall;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return T_REXRCTrackLeftHalfBankedHelixUpLarge;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return T_REXRCTrackRightHalfBankedHelixUpLarge;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return T_REXRCTrackLeftHalfBankedHelixDownLarge;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return T_REXRCTrackRightHalfBankedHelixDownLarge;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return T_REXRCTrackLeftQuarterTurn160DegUp;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return T_REXRCTrackRightQuarterTurn160DegUp;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return T_REXRCTrackLeftQuarterTurn160DegDown;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return T_REXRCTrackRightQuarterTurn160DegDown;
        case TrackElemType::Brakes:
            return T_REXRCTrackBrakes;
        case TrackElemType::Up25LeftBanked:
            return T_REXRCTrack25DegUpLeftBanked;
        case TrackElemType::Up25RightBanked:
            return T_REXRCTrack25DegUpRightBanked;
        case TrackElemType::OnRidePhoto:
            return T_REXRCTrackOnRidePhoto;
        case TrackElemType::Down25LeftBanked:
            return T_REXRCTrack25DegDownLeftBanked;
        case TrackElemType::Down25RightBanked:
            return T_REXRCTrack25DegDownRightBanked;
        case TrackElemType::FlatToUp60LongBase:
            return T_REXRCTrackFlatTo60DegUpLongBase;
        case TrackElemType::Up60ToFlatLongBase:
            return T_REXRCTrack60DegUpToFlatLongBase;
        case TrackElemType::Down60ToFlatLongBase:
            return T_REXRCTrack60DegDownToFlatLongBase;
        case TrackElemType::FlatToDown60LongBase:
            return T_REXRCTrackFlatTo60DegDownLongBase;
        case TrackElemType::CableLiftHill:
            return T_REXRCTrackCableLiftHill;
        case TrackElemType::LeftEighthToDiag:
            return T_REXRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return T_REXRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return T_REXRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return T_REXRCTrackRightEighthToOrthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return T_REXRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return T_REXRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return T_REXRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return T_REXRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlat:
            return T_REXRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return T_REXRCTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return T_REXRCTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return T_REXRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return T_REXRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return T_REXRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return T_REXRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return T_REXRCTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return T_REXRCTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return T_REXRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return T_REXRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return T_REXRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return T_REXRCTrackDiag25DegDownToFlat;
        case TrackElemType::DiagFlatToLeftBank:
            return T_REXRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return T_REXRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return T_REXRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return T_REXRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return T_REXRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return T_REXRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return T_REXRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return T_REXRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return T_REXRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return T_REXRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return T_REXRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return T_REXRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return T_REXRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return T_REXRCTrackDiagRightBank;
        case TrackElemType::BlockBrakes:
            return T_REXRCTrackBlockBrakes;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
            return T_REXRCTrackLeftBankedQuarterTurn325DegUp;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
            return T_REXRCTrackRightBankedQuarterTurn325DegUp;
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return T_REXRCTrackLeftBankedQuarterTurn325DegDown;
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return T_REXRCTrackRightBankedQuarterTurn325DegDown;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
            return T_REXRCTrackLeftBankedQuarterTurn525DegUp;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
            return T_REXRCTrackRightBankedQuarterTurn525DegUp;
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return T_REXRCTrackLeftBankedQuarterTurn525DegDown;
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return T_REXRCTrackRightBankedQuarterTurn525DegDown;
        case TrackElemType::Up25ToLeftBankedUp25:
            return T_REXRCTrack25DegUpToLeftBanked25DegUp;
        case TrackElemType::Up25ToRightBankedUp25:
            return T_REXRCTrack25DegUpToRightBanked25DegUp;
        case TrackElemType::LeftBankedUp25ToUp25:
            return T_REXRCTrackLeftBanked25DegUpTo25DegUp;
        case TrackElemType::RightBankedUp25ToUp25:
            return T_REXRCTrackRightBanked25DegUpTo25DegUp;
        case TrackElemType::Down25ToLeftBankedDown25:
            return T_REXRCTrack25DegDownToLeftBanked25DegDown;
        case TrackElemType::Down25ToRightBankedDown25:
            return T_REXRCTrack25DegDownToRightBanked25DegDown;
        case TrackElemType::LeftBankedDown25ToDown25:
            return T_REXRCTrackLeftBanked25DegDownTo25DegDown;
        case TrackElemType::RightBankedDown25ToDown25:
            return T_REXRCTrackRightBanked25DegDownTo25DegDown;
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
            return T_REXRCTrackLeftBankedFlatToLeftBanked25DegUp;
        case TrackElemType::RightBankedFlatToRightBankedUp25:
            return T_REXRCTrackRightBankedFlatToRightBanked25DegUp;
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
            return T_REXRCTrackLeftBanked25DegUpToLeftBankedFlat;
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
            return T_REXRCTrackRightBanked25DegUpToRightBankedFlat;
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
            return T_REXRCTrackLeftBankedFlatToLeftBanked25DegDown;
        case TrackElemType::RightBankedFlatToRightBankedDown25:
            return T_REXRCTrackRightBankedFlatToRightBanked25DegDown;
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
            return T_REXRCTrackLeftBanked25DegDownToLeftBankedFlat;
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
            return T_REXRCTrackRightBanked25DegDownToRightBankedFlat;
        case TrackElemType::FlatToLeftBankedUp25:
            return T_REXRCTrackFlatToLeftBanked25DegUp;
        case TrackElemType::FlatToRightBankedUp25:
            return T_REXRCTrackFlatToRightBanked25DegUp;
        case TrackElemType::LeftBankedUp25ToFlat:
            return T_REXRCTrackLeftBanked25DegUpToFlat;
        case TrackElemType::RightBankedUp25ToFlat:
            return T_REXRCTrackRightBanked25DegUpToFlat;
        case TrackElemType::FlatToLeftBankedDown25:
            return T_REXRCTrackFlatToLeftBanked25DegDown;
        case TrackElemType::FlatToRightBankedDown25:
            return T_REXRCTrackFlatToRightBanked25DegDown;
        case TrackElemType::LeftBankedDown25ToFlat:
            return T_REXRCTrackLeftBanked25DegDownToFlat;
        case TrackElemType::RightBankedDown25ToFlat:
            return T_REXRCTrackRightBanked25DegDownToFlat;

        case TrackElemType::Booster:
            return T_REXRCTrackBooster;
        case TrackElemType::PoweredLift:
            return T_REXRCTrackPoweredLift;
    }
    return nullptr;
}
