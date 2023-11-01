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
static constexpr uint32_t GIGA_COASTER_BRAKE_SW_NE_OPEN_1 = 18076;
static constexpr uint32_t GIGA_COASTER_BRAKE_NW_SE_OPEN_1 = 18077;
static constexpr uint32_t GIGA_COASTER_BRAKE_SW_NE_CLOSED_1 = 18078;
static constexpr uint32_t GIGA_COASTER_BRAKE_NW_SE_CLOSED_1 = 18079;
static constexpr uint32_t GIGA_COASTER_BRAKE_SW_NE_OPEN_2 = 18080;
static constexpr uint32_t GIGA_COASTER_BRAKE_NW_SE_OPEN_2 = 18081;
static constexpr uint32_t GIGA_COASTER_BRAKE_SW_NE_CLOSED_2 = 18082;
static constexpr uint32_t GIGA_COASTER_BRAKE_NW_SE_CLOSED_2 = 18083;

static constexpr uint32_t _GigaCoasterBrakeImages[NumOrthogonalDirections][2][2] = {
    { { GIGA_COASTER_BRAKE_SW_NE_OPEN_1, GIGA_COASTER_BRAKE_SW_NE_OPEN_2 },
      { GIGA_COASTER_BRAKE_SW_NE_CLOSED_1, GIGA_COASTER_BRAKE_SW_NE_CLOSED_2 } },
    { { GIGA_COASTER_BRAKE_NW_SE_OPEN_1, GIGA_COASTER_BRAKE_NW_SE_OPEN_2 },
      { GIGA_COASTER_BRAKE_NW_SE_CLOSED_1, GIGA_COASTER_BRAKE_NW_SE_CLOSED_2 } },
    { { GIGA_COASTER_BRAKE_SW_NE_OPEN_1, GIGA_COASTER_BRAKE_SW_NE_OPEN_2 },
      { GIGA_COASTER_BRAKE_SW_NE_CLOSED_1, GIGA_COASTER_BRAKE_SW_NE_CLOSED_2 } },
    { { GIGA_COASTER_BRAKE_NW_SE_OPEN_1, GIGA_COASTER_BRAKE_NW_SE_OPEN_2 },
      { GIGA_COASTER_BRAKE_NW_SE_CLOSED_1, GIGA_COASTER_BRAKE_NW_SE_CLOSED_2 } },
};

static void InMaTriangleTrackAltBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_GIGA_RC_BRAKE_ALT_NE_SW),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_GIGA_RC_BRAKE_ALT_SW_NE),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(GIGA_COASTER_BRAKE_NW_SE_CLOSED_1),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(GIGA_COASTER_BRAKE_NW_SE_CLOSED_2),
                { 0, 0, height }, { { 0, 27, height + 5 }, { 32, 1, 11 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, MetalSupportType::Tubes, MetalSupportPlace::Centre, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void InMaTriangleTrackAltBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isClosed = trackElement.IsBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_GigaCoasterBrakeImages[direction][isClosed][0]),
        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_GigaCoasterBrakeImages[direction][isClosed][1]),
        { 0, 0, height }, { { 0, 27, height + 5 }, { 32, 1, 11 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, MetalSupportType::Tubes, MetalSupportPlace::Centre, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionInMaTriangleAlt(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Brakes:
            return InMaTriangleTrackAltBrakes;
        case TrackElemType::BlockBrakes:
            return InMaTriangleTrackAltBlockBrakes;
    }
    return GetTrackPaintFunctionInMaTriangle(trackType);
}
