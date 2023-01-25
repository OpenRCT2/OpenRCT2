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

static constexpr const uint32_t STAND_UP_BLOCK_BRAKE_SW_NE_OPEN = 25571;
static constexpr const uint32_t STAND_UP_BLOCK_BRAKE_NW_SE_OPEN = 25572;
static constexpr const uint32_t STAND_UP_BLOCK_BRAKE_SW_NE_CLOSED = 25573;
static constexpr const uint32_t STAND_UP_BLOCK_BRAKE_NW_SE_CLOSED = 25574;

static constexpr const uint32_t _StandUpBlockBrakeImages[NumOrthogonalDirections][2] = {
    { STAND_UP_BLOCK_BRAKE_SW_NE_OPEN, STAND_UP_BLOCK_BRAKE_SW_NE_CLOSED },
    { STAND_UP_BLOCK_BRAKE_NW_SE_OPEN, STAND_UP_BLOCK_BRAKE_NW_SE_CLOSED },
    { STAND_UP_BLOCK_BRAKE_SW_NE_OPEN, STAND_UP_BLOCK_BRAKE_SW_NE_CLOSED },
    { STAND_UP_BLOCK_BRAKE_NW_SE_OPEN, STAND_UP_BLOCK_BRAKE_NW_SE_CLOSED },
};

/** rct2: 0x008A7114 */
static void StandUpRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25383), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25384), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25453), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25454), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25229), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25230), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A7384, 0x008A7394, 0x008A73A4 */
static void StandUpRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { 25567, SPR_STATION_BASE_A_SW_NE },
        { 25568, SPR_STATION_BASE_A_NW_SE },
        { 25567, SPR_STATION_BASE_A_SW_NE },
        { 25568, SPR_STATION_BASE_A_NW_SE },
    };

    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        bool isClosed = trackElement.IsBrakeClosed();
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_StandUpBlockBrakeImages[direction][isClosed]),
            { 0, 6, height }, { 32, 20, 1 }, { 0, 6, height + 3 });
    }
    else
    {
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]), { 0, 6, height },
            { 32, 20, 1 }, { 0, 6, height + 3 });
    }
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][1]), { 0, 0, height },
        { 32, 32, 1 });
    TrackPaintUtilDrawStationMetalSupports2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 0);
    TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, 9, 11);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A7124 */
static void StandUpRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25385), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25397), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25389), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25399), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25231), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25243), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25235), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25245), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A7134 */
static void StandUpRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25386), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25398), { 0, 6, height }, { 1, 10, 75 },
                    { 29, 10, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25390), { 0, 6, height }, { 1, 10, 75 },
                    { 29, 10, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25400), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25232), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25244), { 0, 6, height }, { 1, 10, 75 },
                    { 29, 10, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25236), { 0, 6, height }, { 1, 10, 75 },
                    { 29, 10, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25246), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008A7144 */
static void StandUpRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25387), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25401), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25391), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25403), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25233), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25247), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25237), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25249), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A7154 */
static void StandUpRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25388), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25411), { 0, 6, height }, { 32, 10, 43 },
                    { 0, 10, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25402), { 0, 6, height }, { 32, 2, 43 },
                    { 0, 4, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25409), { 0, 6, height }, { 32, 10, 43 },
                    { 0, 10, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25392), { 0, 6, height }, { 32, 2, 43 },
                    { 0, 4, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25404), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25234), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25277), { 0, 6, height }, { 32, 10, 43 },
                    { 0, 10, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25248), { 0, 6, height }, { 32, 2, 43 },
                    { 0, 4, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25275), { 0, 6, height }, { 32, 10, 43 },
                    { 0, 10, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25238), { 0, 6, height }, { 32, 2, 43 },
                    { 0, 4, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25250), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A7164 */
static void StandUpRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25396), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25412), { 0, 6, height }, { 32, 10, 43 },
                    { 0, 10, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25408), { 0, 6, height }, { 32, 2, 43 },
                    { 0, 4, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25410), { 0, 6, height }, { 32, 10, 43 },
                    { 0, 10, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25394), { 0, 6, height }, { 32, 2, 43 },
                    { 0, 4, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25406), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25242), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25278), { 0, 6, height }, { 32, 10, 43 },
                    { 0, 10, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25254), { 0, 6, height }, { 32, 2, 43 },
                    { 0, 4, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25276), { 0, 6, height }, { 32, 10, 43 },
                    { 0, 10, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25240), { 0, 6, height }, { 32, 2, 43 },
                    { 0, 4, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25252), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A7174 */
static void StandUpRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25395), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25407), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25393), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25405), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25241), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25253), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25239), { 0, 6, height }, { 32, 20, 3 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25251), { 0, 6, height }, { 32, 20, 3 });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_12);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008A7184 */
static void StandUpRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7194 */
static void StandUpRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A71A4 */
static void StandUpRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A71B4 */
static void StandUpRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A71C4 */
static void StandUpRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A71D4 */
static void StandUpRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A71E4 */
static void StandUpRCTrackLeftQuarterTurn5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25264), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25269), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25274), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25259), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25263), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25268), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25273), { 0, 16, height },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25258), { 0, 16, height },
                        { 32, 16, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25262), { 0, 16, height },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25267), { 16, 16, height },
                        { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25272), { 16, 0, height },
                        { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25257), { 0, 0, height },
                        { 16, 16, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25261), { 16, 0, height },
                        { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25266), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25271), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25256), { 16, 0, height },
                        { 16, 32, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25260), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25265), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25270), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25255), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A71F4 */
static void StandUpRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    StandUpRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7204 */
static void StandUpRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25281), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25286), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25280), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25283), { 0, 6, height }, { 32, 20, 3 });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A7214 */
static void StandUpRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25279), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25284), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25282), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25285), { 0, 6, height }, { 32, 20, 3 });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A7224 */
static void StandUpRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25282), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25285), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25279), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25284), { 0, 6, height }, { 32, 20, 3 });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A7234 */
static void StandUpRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25280), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25283), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25281), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25286), { 0, 6, height }, { 32, 20, 3 });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A7244 */
static void StandUpRCTrackBankedLeftQuarterTurn5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25464), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25475), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25469), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25474), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25459), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25463), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25468), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25473), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25458), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25462), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25467), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25472), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25457), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25461), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25466), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25471), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25456), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25460), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25465), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25470), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25476), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25455), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7254 */
static void StandUpRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    StandUpRCTrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7264 */
static void StandUpRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25315), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25316), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25317), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25318), { 0, 6, height }, { 32, 20, 3 });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A7274 */
static void StandUpRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25319), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25320), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25321), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25322), { 0, 6, height }, { 32, 20, 3 });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A7284 */
static void StandUpRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25311), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25312), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25313), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25314), { 0, 6, height }, { 32, 20, 3 });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_12);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008A7294 */
static void StandUpRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25307), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25308), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25309), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25310), { 0, 6, height }, { 32, 20, 3 });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_12);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008A72A4 */
static void StandUpRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A72B4 */
static void StandUpRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A72C4 */
static void StandUpRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A72D4 */
static void StandUpRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A72E4 */
static void StandUpRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25323), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25324), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25325), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25326), { 0, 6, height }, { 32, 20, 3 });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A72F4 */
static void StandUpRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7304 */
static void StandUpRCTrackLeftQuarterTurn525DegUp(
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
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25433), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25438), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25443), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25448), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25347), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25352), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25357), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25362), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25434), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25439), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25444), { 0, 16, height },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25449), { 0, 16, height },
                            { 32, 16, 3 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25348), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25353), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25358), { 0, 16, height },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25363), { 0, 16, height },
                            { 32, 16, 3 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25435), { 0, 16, height },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25440), { 16, 16, height },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25445), { 16, 0, height },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25450), { 0, 0, height },
                            { 16, 16, 3 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25349), { 0, 16, height },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25354), { 16, 16, height },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25359), { 16, 0, height },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25364), { 0, 0, height },
                            { 16, 16, 3 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25436), { 16, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25441), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25446), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25451), { 16, 0, height },
                            { 16, 32, 3 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25350), { 16, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25355), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25360), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25365), { 16, 0, height },
                            { 16, 32, 3 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25437), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25442), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25447), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25452), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25351), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25356), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25361), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25366), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7314 */
static void StandUpRCTrackRightQuarterTurn525DegUp(
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
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25413), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25418), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25423), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25428), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25327), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25332), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25337), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25342), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25414), { 0, 16, height },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25419), { 0, 16, height },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25424), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25429), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25328), { 0, 16, height },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25333), { 0, 16, height },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25338), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25343), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25415), { 0, 0, height },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25420), { 16, 0, height },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25425), { 16, 16, height },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25430), { 0, 16, height },
                            { 16, 16, 3 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25329), { 0, 0, height },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25334), { 16, 0, height },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25339), { 16, 16, height },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25344), { 0, 16, height },
                            { 16, 16, 3 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25416), { 16, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25421), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25426), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25431), { 16, 0, height },
                            { 16, 32, 3 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25330), { 16, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25335), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25340), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25345), { 16, 0, height },
                            { 16, 32, 3 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25417), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25422), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25427), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25432), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25331), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25336), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25341), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25346), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7324 */
static void StandUpRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    StandUpRCTrackRightQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7334 */
static void StandUpRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    StandUpRCTrackLeftQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7344 */
static void StandUpRCTrackSBendLeft(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25371), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25378), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25374), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25375), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25372), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25377), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25373), { 0, 6, height },
                        { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25376), { 0, 6, height },
                        { 32, 26, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25373), { 0, 6, height },
                        { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25376), { 0, 6, height },
                        { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25372), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25377), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25374), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25375), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25371), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25378), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7354 */
static void StandUpRCTrackSBendRight(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25367), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25382), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25370), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25379), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25368), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25381), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25369), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25380), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25369), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25380), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25368), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25381), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25370), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25379), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25367), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25382), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7364 */
static void StandUpRCTrackLeftVerticalLoop(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25477), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25485), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25484), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25492), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25478), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25486), { 0, 14, height },
                        { 32, 2, 63 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25483), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25491), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25479), { 16, 0, height },
                        { 5, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25487), { 12, 0, height },
                        { 3, 16, 119 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25482), { 10, 16, height },
                        { 4, 16, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25490), { 16, 16, height },
                        { 4, 16, 119 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25480), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25488), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25481), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25489), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25481), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25489), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25480), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25488), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25482), { 10, 16, height },
                        { 4, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25490), { 16, 16, height },
                        { 4, 16, 119 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25479), { 16, 0, height },
                        { 5, 16, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25487), { 12, 0, height },
                        { 3, 16, 119 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25483), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25491), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25478), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25486), { 0, 14, height },
                        { 32, 2, 63 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25484), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25492), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25477), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25485), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, TUNNEL_1);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_1);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A7374 */
static void StandUpRCTrackRightVerticalLoop(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25508), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25500), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25501), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25493), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25507), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25499), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25502), { 0, 14, height },
                        { 32, 2, 63 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25494), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25506), { 16, 16, height },
                        { 4, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25498), { 10, 16, height },
                        { 4, 16, 119 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25503), { 12, 0, height },
                        { 3, 16, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25495), { 16, 0, height },
                        { 5, 16, 119 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25505), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25497), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25504), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25496), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25504), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25496), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25505), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25497), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25503), { 12, 0, height },
                        { 3, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25495), { 16, 0, height },
                        { 5, 16, 119 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25506), { 16, 16, height },
                        { 4, 16, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25498), { 10, 16, height },
                        { 4, 16, 119 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25502), { 0, 14, height },
                        { 32, 2, 63 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25494), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25507), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25499), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25501), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25493), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25508), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25500), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, TUNNEL_1);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_1);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }

    TrackPaintUtilRightVerticalLoopSegments(session, direction, trackSequence);
}

/** rct2: 0x008A73B4 */
static void StandUpRCTrackLeftQuarterTurn3(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25514), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25517), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25520), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25511), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25513), { 16, 0, height },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25516), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25519), { 0, 16, height },
                        { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25510), { 16, 16, height },
                        { 16, 16, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25512), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25515), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25518), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25509), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A73C4 */
static void StandUpRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    StandUpRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A73D4 */
static void StandUpRCTrackLeftQuarterTurn3Bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25526), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25533), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25529), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25532), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25523), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25525), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25528), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25531), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25522), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25524), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25527), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25530), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25534), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25521), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A73E4 */
static void StandUpRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    StandUpRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A73F4 */
static void StandUpRCTrackLeftQuarterTurn325DegUp(
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
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25562), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25564), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25566), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25560), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25546), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25548), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25550), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25544), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
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
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25561), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25563), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25565), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25559), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25545), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25547), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25549), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25543), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7404 */
static void StandUpRCTrackRightQuarterTurn325DegUp(
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
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25551), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25553), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25555), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25557), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25535), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25537), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25539), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25541), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
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
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25552), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25554), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25556), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25558), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25536), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25538), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25540), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25542), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7414 */
static void StandUpRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    StandUpRCTrackRightQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7424 */
static void StandUpRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    StandUpRCTrackLeftQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7434 */
static void StandUpRCTrackHalfLoopUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25575), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25583), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25582), { 0, 6, height },
                        { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25590), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25576), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25584), { 0, 14, height },
                        { 32, 2, 63 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25581), { 0, 6, height }, { 32, 2, 3 },
                        { 0, 20, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25589), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25577), { 16, 0, height },
                        { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25585), { 12, 0, height },
                        { 3, 16, 119 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25580), { 10, 16, height },
                        { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25588), { 16, 16, height },
                        { 2, 16, 119 }, { 15, 6, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25578), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25586), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25579), { 0, 16, height + 32 },
                        { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25587), { 0, 16, height + 32 },
                        { 32, 12, 3 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A7444 */
static void StandUpRCTrackHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrackHalfLoopUp(session, ride, 3 - trackSequence, direction, height, trackElement);
}

/** rct2: 0x008A7454 */
static void StandUpRCTrackLeftCorkscrewUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25591), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25594), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25597), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25600), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
            }

            TrackPaintUtilLeftCorkscrewUpSupports(session, direction, height);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25592), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25595), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25598), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25601), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25593), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25596), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25599), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25602), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A7464 */
static void StandUpRCTrackRightCorkscrewUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25603), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25606), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25609), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25612), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25604), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25607), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25610), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25613), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25605), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25608), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25611), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25614), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A7474 */
static void StandUpRCTrackLeftCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrackRightCorkscrewUp(session, ride, 2 - trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7484 */
static void StandUpRCTrackRightCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    StandUpRCTrackLeftCorkscrewUp(session, ride, 2 - trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7734 */
static void StandUpRCTrackLeftHalfBankedHelixUpSmall(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25844), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25851), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25847), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25850), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25841), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25843), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25846), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25849), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25840), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25842), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25845), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25848), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25852), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25839), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25841), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25844), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25851), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25847), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25850), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25840), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25843), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25846), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25849), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25839), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25842), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25845), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25848), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25852), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7744 */
static void StandUpRCTrackRightHalfBankedHelixUpSmall(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25825), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25828), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25831), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25834), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25838), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25826), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25829), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25832), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25835), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25827), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25830), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25837), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25833), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25836), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25828), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25831), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25834), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25838), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25825), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25829), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25832), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25835), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25826), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25830), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25837), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25833), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25836), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25827), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7754 */
static void StandUpRCTrackLeftHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    StandUpRCTrackRightHalfBankedHelixUpSmall(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7764 */
static void StandUpRCTrackRightHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    StandUpRCTrackLeftHalfBankedHelixUpSmall(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A76F4 */
static void StandUpRCTrackLeftHalfBankedHelixUpLarge(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25812), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25823), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25817), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25822), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25807), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25811), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25816), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25821), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25806), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25810), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25815), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25820), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25805), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25809), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25814), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25819), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25804), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25808), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25813), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25818), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25824), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25803), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25807), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25812), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25823), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25817), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25822), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25806), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25811), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25816), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25821), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25805), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25810), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25815), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25820), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25804), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25809), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25814), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25819), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25803), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25808), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25813), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25818), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25824), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7704 */
static void StandUpRCTrackRightHalfBankedHelixUpLarge(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25781), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25786), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25791), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25796), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25802), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25782), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25787), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25792), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25797), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25783), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25788), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25793), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25798), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25784), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25789), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25794), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25799), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25785), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25790), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25801), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25795), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25800), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25786), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25791), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25796), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25802), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25781), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25787), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25792), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25797), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25782), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25788), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25793), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25798), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25783), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25789), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25794), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25799), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25784), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25790), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25801), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25795), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25800), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25785), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7714 */
static void StandUpRCTrackLeftHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    StandUpRCTrackRightHalfBankedHelixUpLarge(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7724 */
static void StandUpRCTrackRightHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    StandUpRCTrackLeftHalfBankedHelixUpLarge(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7494 */
static void StandUpRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25569), { 0, 6, height }, { 32, 20, 3 });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25570), { 0, 6, height }, { 32, 20, 3 });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A74A4 */
static void StandUpRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25229), { 0, 6, height }, { 32, 20, 1 },
                { 0, 6, height + 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25230), { 0, 6, height }, { 32, 20, 1 },
                { 0, 6, height + 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25229), { 0, 6, height }, { 32, 20, 1 },
                { 0, 6, height + 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25230), { 0, 6, height }, { 32, 20, 1 },
                { 0, 6, height + 3 });
            break;
    }
    TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A74B4 */
static void StandUpRCTrackLeftEighthToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25643), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25647), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25651), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25655), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25644), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25648), { 0, 0, height },
                        { 34, 16, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25652), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25656), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25645), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25649), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25653), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25657), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25646), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25650), { 0, 0, height },
                        { 16, 18, 3 }, { 0, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25654), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25658), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A74C4 */
static void StandUpRCTrackRightEighthToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25627), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25631), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25635), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25639), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25628), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25632), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25636), { 0, 0, height },
                        { 34, 16, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25640), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25629), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25633), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25637), { 0, 0, height },
                        { 28, 28, 3 }, { 4, 4, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25641), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25630), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25634), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25638), { 0, 0, height },
                        { 16, 18, 3 }, { 0, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25642), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A74D4 */
static void StandUpRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    StandUpRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A74E4 */
static void StandUpRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    StandUpRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008A76A4 */
static void StandUpRCTrackLeftEighthBankToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25675), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25679), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25683), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25687), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25676), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25680), { 0, 0, height },
                        { 34, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25684), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25688), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25677), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25681), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25685), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25689), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25678), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25682), { 0, 0, height },
                        { 16, 18, 0 }, { 0, 16, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25686), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25690), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A76B4 */
static void StandUpRCTrackRightEighthBankToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25659), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25663), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25667), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25671), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25660), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25664), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25668), { 0, 0, height },
                        { 34, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25672), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25661), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25665), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25669), { 0, 0, height },
                        { 28, 28, 0 }, { 4, 4, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25673), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25662), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25666), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25670), { 0, 0, height },
                        { 16, 18, 0 }, { 0, 16, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25674), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A76C4 */
static void StandUpRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    StandUpRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A76D4 */
static void StandUpRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    StandUpRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008A74F4 */
static void StandUpRCTrackDiagFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25756), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25694), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25753), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25691), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25755), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25693), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25754), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25692), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7524 */
static void StandUpRCTrackDiag25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25768), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25706), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25765), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25703), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25767), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25705), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25766), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25704), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A7584 */
static void StandUpRCTrackDiag60DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25780), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25718), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25777), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25715), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25779), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25717), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25778), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25716), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008A7504 */
static void StandUpRCTrackDiagFlatTo25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25760), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25698), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25757), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25695), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25759), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25697), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25758), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25696), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A7564 */
static void StandUpRCTrackDiag25DegUpTo60DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25772), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25710), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25769), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25707), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25771), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25709), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25770), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25708), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7574 */
static void StandUpRCTrackDiag60DegUpTo25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25776), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25714), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25773), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25711), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25775), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25713), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25774), { -16, -16, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25712), { -16, -16, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7514 */
static void StandUpRCTrackDiag25DegUpToFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25764), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25702), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25761), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25699), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25763), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25701), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25762), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25700), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A7554 */
static void StandUpRCTrackDiag25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25766), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25704), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25767), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25705), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25765), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25703), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25768), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25706), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A75B4 */
static void StandUpRCTrackDiag60DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25778), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25716), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25779), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25717), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25777), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25715), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25780), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25718), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008A7534 */
static void StandUpRCTrackDiagFlatTo25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25762), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25700), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25763), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25701), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25761), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25699), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25764), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25702), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A7594 */
static void StandUpRCTrackDiag25DegDownTo60DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25774), { -16, -16, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25712), { -16, -16, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25775), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25713), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25773), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25711), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25776), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25714), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A75A4 */
static void StandUpRCTrackDiag60DegDownTo25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25770), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25708), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25771), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25709), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25769), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25707), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25772), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25710), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7544 */
static void StandUpRCTrackDiag25DegDownToFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25758), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25696), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25759), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25697), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25757), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25695), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
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
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25760), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25698), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A75E4 */
static void StandUpRCTrackDiagFlatToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25726), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25723), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25727), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25725), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25724), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A75F4 */
static void StandUpRCTrackDiagFlatToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25731), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25728), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25730), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25732), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
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
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25729), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7604 */
static void StandUpRCTrackDiagLeftBankToFlat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25729), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25730), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25732), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25728), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25731), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7614 */
static void StandUpRCTrackDiagRightBankToFlat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25724), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25725), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25723), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25727), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
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
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25726), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7644 */
static void StandUpRCTrackDiagLeftBankTo25DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25746), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25743), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25747), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25745), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25744), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A7654 */
static void StandUpRCTrackDiagRightBankTo25DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25751), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25748), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25750), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25752), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
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
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25749), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A7624 */
static void StandUpRCTrackDiag25DegUpToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25736), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25733), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25737), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25735), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25734), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A7634 */
static void StandUpRCTrackDiag25DegUpToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25741), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25738), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25740), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25742), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
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
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25739), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A7664 */
static void StandUpRCTrackDiagLeftBankTo25DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25739), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25740), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25742), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25738), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25741), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A7674 */
static void StandUpRCTrackDiagRightBankTo25DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25734), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25735), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16676), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16680), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
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
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25736), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A7684 */
static void StandUpRCTrackDiag25DegDownToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25749), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25750), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25752), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25748), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25751), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A7694 */
static void StandUpRCTrackDiag25DegDownToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25744), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25745), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25743), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25747), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
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
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25746), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A75C4 */
static void StandUpRCTrackDiagLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25722), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25719), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25721), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25720), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A75D4 */
static void StandUpRCTrackDiagRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25720), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25721), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25719), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
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
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25722), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A76E4 */
static void StandUpRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isClosed = trackElement.IsBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_StandUpBlockBrakeImages[direction][isClosed]),
        { 0, 6, height }, { 32, 20, 3 });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionStandUpRC(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return StandUpRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return StandUpRCTrackStation;
        case TrackElemType::Up25:
            return StandUpRCTrack25DegUp;
        case TrackElemType::Up60:
            return StandUpRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return StandUpRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return StandUpRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return StandUpRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return StandUpRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return StandUpRCTrack25DegDown;
        case TrackElemType::Down60:
            return StandUpRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return StandUpRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return StandUpRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return StandUpRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return StandUpRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return StandUpRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return StandUpRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return StandUpRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return StandUpRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return StandUpRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return StandUpRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return StandUpRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return StandUpRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return StandUpRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return StandUpRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return StandUpRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return StandUpRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return StandUpRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return StandUpRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return StandUpRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return StandUpRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return StandUpRCTrackLeftBank;
        case TrackElemType::RightBank:
            return StandUpRCTrackRightBank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return StandUpRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return StandUpRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return StandUpRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return StandUpRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::SBendLeft:
            return StandUpRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return StandUpRCTrackSBendRight;
        case TrackElemType::LeftVerticalLoop:
            return StandUpRCTrackLeftVerticalLoop;
        case TrackElemType::RightVerticalLoop:
            return StandUpRCTrackRightVerticalLoop;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return StandUpRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return StandUpRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return StandUpRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return StandUpRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return StandUpRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return StandUpRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return StandUpRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return StandUpRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::HalfLoopUp:
            return StandUpRCTrackHalfLoopUp;
        case TrackElemType::HalfLoopDown:
            return StandUpRCTrackHalfLoopDown;
        case TrackElemType::LeftCorkscrewUp:
            return StandUpRCTrackLeftCorkscrewUp;
        case TrackElemType::RightCorkscrewUp:
            return StandUpRCTrackRightCorkscrewUp;
        case TrackElemType::LeftCorkscrewDown:
            return StandUpRCTrackLeftCorkscrewDown;
        case TrackElemType::RightCorkscrewDown:
            return StandUpRCTrackRightCorkscrewDown;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return StandUpRCTrackLeftHalfBankedHelixUpSmall;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return StandUpRCTrackRightHalfBankedHelixUpSmall;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return StandUpRCTrackLeftHalfBankedHelixDownSmall;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return StandUpRCTrackRightHalfBankedHelixDownSmall;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return StandUpRCTrackLeftHalfBankedHelixUpLarge;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return StandUpRCTrackRightHalfBankedHelixUpLarge;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return StandUpRCTrackLeftHalfBankedHelixDownLarge;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return StandUpRCTrackRightHalfBankedHelixDownLarge;
        case TrackElemType::Brakes:
            return StandUpRCTrackBrakes;
        case TrackElemType::OnRidePhoto:
            return StandUpRCTrackOnRidePhoto;
        case TrackElemType::LeftEighthToDiag:
            return StandUpRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return StandUpRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return StandUpRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return StandUpRCTrackRightEighthToOrthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return StandUpRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return StandUpRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return StandUpRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return StandUpRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlat:
            return StandUpRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return StandUpRCTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return StandUpRCTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return StandUpRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return StandUpRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return StandUpRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return StandUpRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return StandUpRCTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return StandUpRCTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return StandUpRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return StandUpRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return StandUpRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return StandUpRCTrackDiag25DegDownToFlat;
        case TrackElemType::DiagFlatToLeftBank:
            return StandUpRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return StandUpRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return StandUpRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return StandUpRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return StandUpRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return StandUpRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return StandUpRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return StandUpRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return StandUpRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return StandUpRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return StandUpRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return StandUpRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return StandUpRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return StandUpRCTrackDiagRightBank;
        case TrackElemType::BlockBrakes:
            return StandUpRCTrackBlockBrakes;
    }
    return nullptr;
}
