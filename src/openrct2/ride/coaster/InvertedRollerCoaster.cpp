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

/** rct2: 0x008A92E8 */
static void InvertedRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27131), { 0, 0, height + 29 },
                    { { 0, 6, height + 29 }, { 32, 20, 3 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27132), { 0, 0, height + 29 },
                    { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27129), { 0, 0, height + 29 },
                    { { 0, 6, height + 29 }, { 32, 20, 3 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27130), { 0, 0, height + 29 },
                    { { 0, 6, height + 29 }, { 32, 20, 3 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A9558, 0x008A9568, 0x008A9578 */
static void InvertedRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 27131, SPR_STATION_INVERTED_BAR_C_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 27132, SPR_STATION_INVERTED_BAR_C_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 27131, SPR_STATION_INVERTED_BAR_C_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 27132, SPR_STATION_INVERTED_BAR_C_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][0]), { 0, 0, height },
        { { 0, 2, height }, { 32, 28, 1 } });
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]), { 0, 0, height + 29 },
        { { 0, 6, height + 29 }, { 32, 20, 3 } });
    PaintAddImageAsChildRotated(
        session, direction, session.TrackColours[SCHEME_SUPPORTS].WithIndex(imageIds[direction][2]), { 0, 6, height + 29 },
        { 32, 20, 3 }, { 0, 6, height + 29 });
    TrackPaintUtilDrawStationMetalSupports2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 3);
    TrackPaintUtilDrawStationInverted(session, ride, direction, height, trackElement, STATION_VARIANT_TALL);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A92F8 */
static void InvertedRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27249), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27250), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27251), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27252), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27221), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27222), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27223), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27224), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A9308 */
static void InvertedRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27237), { 0, 0, height + 29 },
                { { 0, 6, height + 93 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27238), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 81 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27239), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 81 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27240), { 0, 0, height + 29 },
                { { 0, 6, height + 93 }, { 32, 20, 3 } });
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_INVERTED_5);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
}

/** rct2: 0x008A9318 */
static void InvertedRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27241), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27242), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27243), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27244), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27213), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27214), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27215), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27216), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008A9328 */
static void InvertedRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27225), { 0, 0, height + 29 },
                { { 0, 6, height + 61 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27229), { 0, 0, height + 29 },
                { { 0, 10, height + 11 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27226), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 49 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27230), { 0, 0, height + 29 },
                { { 0, 10, height + 11 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27227), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 49 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27228), { 0, 0, height + 29 },
                { { 0, 6, height + 61 }, { 32, 20, 3 } });
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_INVERTED_5);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
}

/** rct2: 0x008A9338 */
static void InvertedRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27231), { 0, 0, height + 29 },
                { { 0, 6, height + 61 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27235), { 0, 0, height + 29 },
                { { 0, 10, height + 11 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27232), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 49 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27236), { 0, 0, height + 29 },
                { { 0, 10, height + 11 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27233), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 49 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27234), { 0, 0, height + 29 },
                { { 0, 6, height + 61 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 76, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 76, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 76, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 76, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
}

/** rct2: 0x008A9348 */
static void InvertedRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27245), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27246), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27247), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27248), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27217), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27218), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27219), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27220), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A9358 */
static void InvertedRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9368 */
static void InvertedRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9378 */
static void InvertedRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9388 */
static void InvertedRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9398 */
static void InvertedRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A93A8 */
static void InvertedRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A93B8 */
static void InvertedRCTrackLeftQuarterTurn5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27142), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27147), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27152), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27137), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27141), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27146), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27151), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27136), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27140), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27145), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27150), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27135), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27139), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27144), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27149), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27134), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27138), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27143), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27148), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27133), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A93C8 */
static void InvertedRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A93D8 */
static void InvertedRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27269), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27270), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27271), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27272), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
        0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A93E8 */
static void InvertedRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27273), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27274), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27275), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27276), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
        0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A93F8 */
static void InvertedRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27275), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27276), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27273), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27274), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
        0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A9408 */
static void InvertedRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27271), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27272), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27269), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27270), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
        0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A9418 */
static void InvertedRCTrackBankedLeftQuarterTurn5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27162), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27167), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27172), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27157), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27161), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27166), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27171), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27156), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27160), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27165), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27170), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27155), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27159), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27164), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27169), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27154), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27158), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27163), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27168), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27153), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9428 */
static void InvertedRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedRCTrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A9438 */
static void InvertedRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27277), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27278), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27279), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27280), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
        0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008A9448 */
static void InvertedRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27281), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27282), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27283), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27284), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
        0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008A9458 */
static void InvertedRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27285), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27286), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27287), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27288), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
        0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A9468 */
static void InvertedRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27289), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27290), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27291), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27292), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
        0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A9478 */
static void InvertedRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9488 */
static void InvertedRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9498 */
static void InvertedRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A94A8 */
static void InvertedRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A94B8 */
static void InvertedRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27293), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27294), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27295), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27296), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
        0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A94C8 */
static void InvertedRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A94D8 */
static void InvertedRCTrackLeftQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27317), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27322), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27327), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27332), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27318), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27323), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27328), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27333), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27319), { 0, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27324), { 16, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27329), { 16, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27334), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27320), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27325), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27330), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27335), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27321), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27326), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27331), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27336), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_INVERTED_5);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_INVERTED_5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A94E8 */
static void InvertedRCTrackRightQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27297), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27302), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27307), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27312), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27298), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27303), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27308), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27313), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27299), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27304), { 16, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27309), { 16, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27314), { 0, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27300), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27305), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27310), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27315), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27301), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27306), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27311), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27316), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_INVERTED_5);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_INVERTED_5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A94F8 */
static void InvertedRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedRCTrackRightQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A9508 */
static void InvertedRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedRCTrackLeftQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A9518 */
static void InvertedRCTrackSBendLeft(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27253), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27257), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27256), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27260), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27254), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27258), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27255), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27259), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 5, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 6, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27255), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27259), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27254), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27258), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 5, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 6, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27256), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27260), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27253), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27257), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9528 */
static void InvertedRCTrackSBendRight(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27261), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27265), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27264), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27268), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27262), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27266), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27263), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27267), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 8, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 7, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27263), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27267), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27262), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27266), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 8, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 7, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27264), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27268), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27261), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27265), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9538 */
static void InvertedRCTrackLeftVerticalLoop(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27355), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27363), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27362), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27370), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 10, 7 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 6, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 8, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 7, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 5, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27356), { 0, 0, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27364), { 0, 14, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27361), { 0, 6, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27369), { 0, 6, height + 29 },
                        { { 29, 20, height + 29 }, { 2, 2, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27357), { 16, 0, height + 2 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27365), { 12, 0, height + 2 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27360), { 10, 16, height + 2 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27368), { 16, 16, height + 2 },
                        { { 29, 29, height + 2 }, { 2, 2, 119 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27358), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27366), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27359), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27367), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27359), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27367), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27358), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27366), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27360), { 10, 16, height + 2 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27368), { 16, 16, height + 2 },
                        { { 29, 29, height + 2 }, { 2, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27357), { 16, 0, height + 2 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27365), { 12, 0, height + 2 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27361), { 0, 6, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27369), { 0, 6, height + 29 },
                        { { 29, 20, height + 29 }, { 2, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27356), { 0, 0, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27364), { 0, 14, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27362), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27370), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 10, 7 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27355), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27363), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 7, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 5, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 6, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 8, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, TUNNEL_INVERTED_4);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_INVERTED_4);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A9548 */
static void InvertedRCTrackRightVerticalLoop(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27386), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 10, 7 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27378), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27379), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27371), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 6, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 8, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 7, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 5, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27385), { 0, 6, height + 29 },
                        { { 29, 20, height + 29 }, { 2, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27377), { 0, 6, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27380), { 0, 14, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27372), { 0, 0, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27384), { 16, 16, height + 2 },
                        { { 29, 29, height + 2 }, { 2, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27376), { 10, 16, height + 2 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27381), { 12, 0, height + 2 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27373), { 16, 0, height + 2 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27383), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27375), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27382), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27374), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27382), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27374), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27383), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27375), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27381), { 12, 0, height + 2 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27373), { 16, 0, height + 2 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27384), { 16, 16, height + 2 },
                        { { 29, 29, height + 2 }, { 2, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27376), { 10, 16, height + 2 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27380), { 0, 14, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27372), { 0, 0, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27385), { 0, 6, height + 29 },
                        { { 29, 20, height + 29 }, { 2, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27377), { 0, 6, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27379), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27371), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27386), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 10, 7 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27378), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 7, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 5, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 6, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 8, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, TUNNEL_INVERTED_4);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_INVERTED_4);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A9588 */
static void InvertedRCTrackLeftQuarterTurn3(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27392), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27395), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27398), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27389), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27391), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27394), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27397), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27388), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27390), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27393), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27396), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27387), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9598 */
static void InvertedRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A95A8 */
static void InvertedRCTrackLeftQuarterTurn3Bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27404), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27407), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27410), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27401), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27403), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27406), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27409), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27400), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27402), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27405), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27408), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27399), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A95B8 */
static void InvertedRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A95C8 */
static void InvertedRCTrackLeftQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27422), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27424), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27426), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27420), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27421), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27423), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27425), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27419), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_INVERTED_5);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_INVERTED_5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A95D8 */
static void InvertedRCTrackRightQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27411), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27413), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27415), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27417), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27412), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27414), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27416), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27418), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_INVERTED_5);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_INVERTED_5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A95E8 */
static void InvertedRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedRCTrackRightQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A95F8 */
static void InvertedRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedRCTrackLeftQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A9608 */
static void InvertedRCTrackLeftTwistDownToUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27427), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27433), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27432), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27438), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27428), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27434), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27431), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27437), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27429), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27435), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27430), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27436), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9618 */
static void InvertedRCTrackRightTwistDownToUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27439), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27445), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27444), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27450), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27440), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27446), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27443), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27449), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27441), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27447), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27442), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27448), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9628 */
static void InvertedRCTrackLeftTwistUpToDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27430), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27436), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27429), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27435), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27431), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27437), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27428), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27434), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27432), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27438), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27427), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27433), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9638 */
static void InvertedRCTrackRightTwistUpToDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27442), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27448), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27441), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27447), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27443), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27449), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27440), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27446), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27444), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27450), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27439), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27445), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9648 */
static void InvertedRCTrackHalfLoopUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27451), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27459), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27458), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27466), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27452), { 0, 0, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27460), { 0, 14, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27457), { 0, 6, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27465), { 0, 6, height + 29 },
                        { { 29, 20, height + 29 }, { 2, 2, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27453), { 16, 0, height + 2 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27461), { 12, 0, height + 2 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27456), { 10, 16, height + 2 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27464), { 16, 16, height + 2 },
                        { { 29, 29, height + 2 }, { 2, 2, 119 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27454), { 0, 0, height + 34 },
                        { { 0, 6, height + 34 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27462), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27455), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27463), { 0, 16, height + 34 },
                        { { 0, 6, height + 34 }, { 32, 20, 3 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 32, TUNNEL_INVERTED_3);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008A9658 */
static void InvertedRCTrackHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackHalfLoopUp(session, ride, 3 - trackSequence, direction, height, trackElement);
}

/** rct2: 0x008A9668 */
static void InvertedRCTrackLeftCorkscrewUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27467), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27470), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27473), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27476), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27468), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27471), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27474), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27477), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27469), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27472), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27475), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27478), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 40, TUNNEL_INVERTED_3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 40, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008A9678 */
static void InvertedRCTrackRightCorkscrewUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27479), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27482), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27485), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27488), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27480), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27483), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27486), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27489), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27481), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27484), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27487), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27490), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 40, TUNNEL_INVERTED_3);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 40, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008A9688 */
static void InvertedRCTrackLeftCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackRightCorkscrewUp(session, ride, 2 - trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A9698 */
static void InvertedRCTrackRightCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackLeftCorkscrewUp(session, ride, 2 - trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A96D8 */
static void InvertedRCTrackLeftQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27352), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27348), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27353), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27349), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27354), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27350), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27351), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27347), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
    }
    TrackPaintUtilLeftQuarterTurn1TileTunnel(session, direction, height, -8, TUNNEL_INVERTED_4, +56, TUNNEL_INVERTED_5);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008A96B8 */
static void InvertedRCTrackRightQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27343), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27339), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27344), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27340), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27345), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27341), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27346), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27342), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
    }
    TrackPaintUtilRightQuarterTurn1TileTunnel(session, direction, height, -8, TUNNEL_INVERTED_4, +56, TUNNEL_INVERTED_5);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008A96C8 */
static void InvertedRCTrackLeftQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackRightQuarterTurn160DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A96E8 */
static void InvertedRCTrackRightQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackLeftQuarterTurn160DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A96A8 */
static void InvertedRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27337), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27338), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A96F8 */
static void InvertedRCTrackLeftQuarterBankedHelixLargeUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27202), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27207), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27212), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27197), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27201), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27206), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27211), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27196), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27200), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27205), { 0, 0, height + 29 },
                        { { 16, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27210), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27195), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27199), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27204), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27209), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27194), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27198), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27203), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27208), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27193), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 58, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 16, TUNNEL_INVERTED_3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 16, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9708 */
static void InvertedRCTrackRightQuarterBankedHelixLargeUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27173), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27178), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27183), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27188), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27174), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27179), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27184), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27189), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27175), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27180), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27185), { 0, 0, height + 29 },
                        { { 16, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27190), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27176), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27181), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27186), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27191), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27177), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27182), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27187), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27192), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 58, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 16, TUNNEL_INVERTED_3);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 16, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9718 */
static void InvertedRCTrackLeftQuarterBankedHelixLargeDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27182), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27187), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27192), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27177), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 58, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27181), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27186), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27191), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27176), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27180), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27185), { 0, 0, height + 29 },
                        { { 16, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27190), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27175), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27179), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27184), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27189), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27174), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27178), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27183), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27188), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27173), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9728 */
static void InvertedRCTrackRightQuarterBankedHelixLargeDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27193), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27198), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27203), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27208), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 58, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27194), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27199), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27204), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27209), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27195), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27200), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27205), { 0, 0, height + 29 },
                        { { 16, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27210), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27196), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27201), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27206), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27211), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27197), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27202), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27207), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27212), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9A38 */
static void InvertedRCTrack25DegUpLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27711), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27712), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27713), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27714), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A9A48 */
static void InvertedRCTrack25DegUpRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27715), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27716), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27717), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27718), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A9738 */
static void InvertedRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27129), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27130), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27129), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27130), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }
    TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008A9A58 */
static void InvertedRCTrack25DegDownLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack25DegUpRightBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9A68 */
static void InvertedRCTrack25DegDownRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack25DegUpLeftBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9748 */
static void InvertedRCTrackLeftEighthToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27507), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27511), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27515), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27519), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27508), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27512), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27516), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27520), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27509), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27513), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27517), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27521), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27510), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27514), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 18, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27518), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27522), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9758 */
static void InvertedRCTrackRightEighthToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27491), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27495), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27499), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27503), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27492), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27496), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27500), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27504), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27493), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27497), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27501), { 0, 0, height + 29 },
                        { { 4, 4, height + 29 }, { 28, 28, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27505), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27494), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27498), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27502), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 18, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27506), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9768 */
static void InvertedRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9778 */
static void InvertedRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008A9938 */
static void InvertedRCTrackLeftEighthBankToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27539), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27543), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27547), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27551), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27540), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27544), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27548), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27552), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27541), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27545), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27549), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27553), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27542), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27546), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 18, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27550), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27554), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9948 */
static void InvertedRCTrackRightEighthBankToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27523), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27527), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27531), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27535), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27524), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27528), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27532), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27536), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27525), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27529), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27533), { 0, 0, height + 29 },
                        { { 4, 4, height + 29 }, { 28, 28, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27537), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27526), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27530), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27534), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 18, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27538), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9958 */
static void InvertedRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9968 */
static void InvertedRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008A9788 */
static void InvertedRCTrackDiagFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27614), { -16, -16, height + 29 },
                            { { -16, -16, height + 29 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27558), { -16, -16, height + 29 },
                            { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27611), { -16, -16, height + 29 },
                            { { -16, -16, height + 29 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27555), { -16, -16, height + 29 },
                            { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27613), { -16, -16, height + 29 },
                            { { -16, -16, height + 29 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27557), { -16, -16, height + 29 },
                            { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27612), { -16, -16, height + 29 },
                            { { -16, -16, height + 29 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27556), { -16, -16, height + 29 },
                            { { -16, -16, height + 29 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A97B8 */
static void InvertedRCTrackDiag25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27626), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27570), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27623), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27567), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27625), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27569), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27624), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27568), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A9818 */
static void InvertedRCTrackDiag60DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27582), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27579), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27581), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27580), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 32, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 36, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 32, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 36, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
    }
}

/** rct2: 0x008A9798 */
static void InvertedRCTrackDiagFlatTo25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27618), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27562), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27615), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27559), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27617), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27561), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27616), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27560), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008A97F8 */
static void InvertedRCTrackDiag25DegUpTo60DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27574), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27571), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27573), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27572), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 16, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 16, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 16, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 16, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A9808 */
static void InvertedRCTrackDiag60DegUpTo25DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27578), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27575), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27577), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27576), { -16, -16, height + 29 },
                        { { 0, 0, height + 61 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 21, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 21, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 21, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 21, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A97A8 */
static void InvertedRCTrackDiag25DegUpToFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27622), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27566), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27619), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27563), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27621), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27565), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27620), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27564), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A97E8 */
static void InvertedRCTrackDiag25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27624), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27568), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27625), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27569), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27623), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27567), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27626), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27570), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A9848 */
static void InvertedRCTrackDiag60DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27580), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27581), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27579), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27582), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 24, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 28, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 24, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 28, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
    }
}

/** rct2: 0x008A97C8 */
static void InvertedRCTrackDiagFlatTo25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27620), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27564), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27621), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27565), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27619), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27563), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27622), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27566), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A9828 */
static void InvertedRCTrackDiag25DegDownTo60DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27576), { -16, -16, height + 29 },
                        { { 0, 0, height + 61 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27577), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27575), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27578), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 17, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 17, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 17, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 17, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A9838 */
static void InvertedRCTrackDiag60DegDownTo25DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27572), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27573), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27571), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27574), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 8, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 8, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 8, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 8, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A97D8 */
static void InvertedRCTrackDiag25DegDownToFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27616), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27560), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27617), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27561), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27615), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27559), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27618), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27562), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008A9878 */
static void InvertedRCTrackDiagFlatToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27590), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27587), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27589), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27588), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9888 */
static void InvertedRCTrackDiagFlatToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27594), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27591), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27593), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27592), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9898 */
static void InvertedRCTrackDiagLeftBankToFlat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27592), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27593), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27591), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27594), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A98A8 */
static void InvertedRCTrackDiagRightBankToFlat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27588), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27589), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27587), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27590), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A98D8 */
static void InvertedRCTrackDiagLeftBankTo25DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27606), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27603), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27605), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27604), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008A98E8 */
static void InvertedRCTrackDiagRightBankTo25DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27610), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27607), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27609), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27608), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008A98B8 */
static void InvertedRCTrackDiag25DegUpToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27598), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27595), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27597), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27596), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A98C8 */
static void InvertedRCTrackDiag25DegUpToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27602), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27599), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27601), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27600), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A98F8 */
static void InvertedRCTrackDiagLeftBankTo25DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27600), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27601), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27599), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27602), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A9908 */
static void InvertedRCTrackDiagRightBankTo25DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27596), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27597), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27595), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27598), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A9918 */
static void InvertedRCTrackDiag25DegDownToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27608), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27609), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27607), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27610), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008A9928 */
static void InvertedRCTrackDiag25DegDownToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27604), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27605), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27603), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27606), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008A9858 */
static void InvertedRCTrackDiagLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27586), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27583), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27585), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27584), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9868 */
static void InvertedRCTrackDiagRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27584), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27585), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27583), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27586), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A9978 */
static void InvertedRCTrackLeftLargeHalfLoopUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27639), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27646), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27653), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27660), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27640), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27647), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 9 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27654), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27661), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27641), { 0, 0, height },
                        { { 8, 0, height + 40 }, { 24, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27648), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27655), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27662), { 0, 0, height },
                        { { 30, 16, height + 70 }, { 2, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27642), { 0, 0, height },
                        { { 24, 0, height + 29 }, { 8, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27649), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27656), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27663), { 0, 0, height },
                        { { 30, 16, height + 100 }, { 2, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 224, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27643), { 0, 0, height },
                        { { 30, 30, height + 80 }, { 2, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27650), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27657), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27664), { 0, 0, height },
                        { { 30, 0, height + 90 }, { 2, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 128, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27644), { 0, 0, height },
                        { { 30, 16, height }, { 2, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27651), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27658), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27665), { 0, 0, height },
                        { { 30, 0, height + 140 }, { 2, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 224, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27645), { 0, 0, height },
                        { { 0, 16, height + 32 }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 4, 27, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27652), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 4, 27, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27659), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 4, 25, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27666), { 0, 0, height },
                        { { 0, 0, height + 32 }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 4, 27, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 32, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
            break;
    }
}

/** rct2: 0x008A9988 */
static void InvertedRCTrackRightLargeHalfLoopUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27688), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27681), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27674), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27667), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27689), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27682), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27675), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 19 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27668), { 0, 0, height },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27690), { 0, 0, height },
                        { { 0, 16, height + 70 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27683), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27676), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27669), { 0, 0, height },
                        { { 0, 0, height + 70 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27691), { 0, 0, height },
                        { { 0, 16, height + 100 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27684), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27677), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27670), { 0, 0, height },
                        { { 0, 0, height + 100 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 224, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27692), { 0, 0, height },
                        { { 16, 0, height + 90 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27685), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27678), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27671), { 0, 0, height },
                        { { 30, 16, height + 110 }, { 2, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 128, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27693), { 0, 0, height },
                        { { 30, 0, height + 140 }, { 2, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27686), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27679), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27672), { 0, 0, height },
                        { { 30, 16, height + 140 }, { 2, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 224, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27694), { 0, 0, height },
                        { { 0, 0, height + 32 }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 4, 27, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27687), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 4, 25, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27680), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 4, 27, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27673), { 0, 0, height },
                        { { 0, 16, height + 32 }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 4, 27, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 32, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
            break;
    }
}

/** rct2: 0x008A9998 */
static void InvertedRCTrackRightLargeHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackRightLargeHalfLoopUp(session, ride, 6 - trackSequence, direction, height, trackElement);
}

/** rct2: 0x008A99A8 */
static void InvertedRCTrackLeftLargeHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackLeftLargeHalfLoopUp(session, ride, 6 - trackSequence, direction, height, trackElement);
}

/** rct2: 0x008A96A8 */
static void InvertedRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27337), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27338), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A9A78 */
static void InvertedRCTrackLeftBankedQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27762), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27764), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27766), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27760), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27761), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27763), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27765), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27759), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_INVERTED_5);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_INVERTED_5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A9A88 */
static void InvertedRCTrackRightBankedQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27751), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27753), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27755), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27757), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27752), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27754), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27756), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27758), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_INVERTED_5);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_INVERTED_5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A9A98 */
static void InvertedRCTrackLeftBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedRCTrackRightBankedQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A9AA8 */
static void InvertedRCTrackRightBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedRCTrackLeftBankedQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A9BB8 */
static void InvertedRCTrackLeftBankedQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27787), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27792), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27797), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27802), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27788), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27793), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27798), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27803), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27789), { 0, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27794), { 16, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27799), { 16, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27804), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27790), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27795), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27800), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27805), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27791), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27796), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27801), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27806), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_INVERTED_5);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_INVERTED_5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A9BC8 */
static void InvertedRCTrackRightBankedQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27767), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27772), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27777), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27782), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27768), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27773), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27778), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27783), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27769), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27774), { 16, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27779), { 16, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27784), { 0, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27770), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27775), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27780), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27785), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27771), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27776), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27781), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27786), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_INVERTED_5);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_INVERTED_5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008A9BD8 */
static void InvertedRCTrackLeftBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedRCTrackRightBankedQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A9BE8 */
static void InvertedRCTrackRightBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedRCTrackLeftBankedQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A9AB8 */
static void InvertedRCTrack25DegUpToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27719), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27720), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27721), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27722), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A9AC8 */
static void InvertedRCTrack25DegUpToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27723), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27724), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27725), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27726), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A9AD8 */
static void InvertedRCTrackLeftBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27727), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27728), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27729), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27730), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A9AE8 */
static void InvertedRCTrackRightBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27731), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27732), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27733), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27734), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A9AF8 */
static void InvertedRCTrack25DegDownToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackRightBanked25DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9B08 */
static void InvertedRCTrack25DegDownToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackLeftBanked25DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9B18 */
static void InvertedRCTrackLeftBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack25DegUpToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9B28 */
static void InvertedRCTrackRightBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrack25DegUpToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9B38 */
static void InvertedRCTrackLeftBankedFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27735), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27736), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27737), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27738), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008A9B48 */
static void InvertedRCTrackRightBankedFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27739), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27740), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27741), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27742), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008A9B58 */
static void InvertedRCTrackLeftBanked25DegUpToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27743), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27744), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27745), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27746), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A9B68 */
static void InvertedRCTrackRightBanked25DegUpToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27747), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27748), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27749), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27750), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A9B78 */
static void InvertedRCTrackLeftBankedFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackRightBanked25DegUpToRightBankedFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9B88 */
static void InvertedRCTrackRightBankedFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackLeftBanked25DegUpToLeftBankedFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9B98 */
static void InvertedRCTrackLeftBanked25DegDownToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackRightBankedFlatToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9BA8 */
static void InvertedRCTrackRightBanked25DegDownToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackLeftBankedFlatToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A99B8 */
static void InvertedRCTrackFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27695), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27696), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27697), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27698), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008A99C8 */
static void InvertedRCTrackFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27699), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27700), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27701), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27702), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008A99D8 */
static void InvertedRCTrackLeftBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27703), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27704), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27705), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27706), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A99E8 */
static void InvertedRCTrackRightBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27707), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27708), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27709), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27710), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 6, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 7, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_INVERTED_3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A99F8 */
static void InvertedRCTrackFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackRightBanked25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9A08 */
static void InvertedRCTrackFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackLeftBanked25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9A18 */
static void InvertedRCTrackLeftBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackFlatToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A9A28 */
static void InvertedRCTrackRightBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    InvertedRCTrackFlatToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

// Added by OpenRCT2

static void InvertedRCTrackBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const auto imageId = (direction & 1) ? SPR_G2_BM_INVERT_BOOSTER_2 : SPR_G2_BM_INVERT_BOOSTER_1;
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageId), { 0, 0, height + 29 },
        { { 0, 6, height + 29 }, { 32, 20, 1 } });

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionInvertedRC(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return InvertedRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return InvertedRCTrackStation;
        case TrackElemType::Up25:
            return InvertedRCTrack25DegUp;
        case TrackElemType::Up60:
            return InvertedRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return InvertedRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return InvertedRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return InvertedRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return InvertedRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return InvertedRCTrack25DegDown;
        case TrackElemType::Down60:
            return InvertedRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return InvertedRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return InvertedRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return InvertedRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return InvertedRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return InvertedRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return InvertedRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return InvertedRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return InvertedRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return InvertedRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return InvertedRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return InvertedRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return InvertedRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return InvertedRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return InvertedRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return InvertedRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return InvertedRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return InvertedRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return InvertedRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return InvertedRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return InvertedRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return InvertedRCTrackLeftBank;
        case TrackElemType::RightBank:
            return InvertedRCTrackRightBank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return InvertedRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return InvertedRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return InvertedRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return InvertedRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::SBendLeft:
            return InvertedRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return InvertedRCTrackSBendRight;
        case TrackElemType::LeftVerticalLoop:
            return InvertedRCTrackLeftVerticalLoop;
        case TrackElemType::RightVerticalLoop:
            return InvertedRCTrackRightVerticalLoop;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return InvertedRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return InvertedRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return InvertedRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return InvertedRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return InvertedRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return InvertedRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return InvertedRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return InvertedRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::LeftTwistDownToUp:
            return InvertedRCTrackLeftTwistDownToUp;
        case TrackElemType::RightTwistDownToUp:
            return InvertedRCTrackRightTwistDownToUp;
        case TrackElemType::LeftTwistUpToDown:
            return InvertedRCTrackLeftTwistUpToDown;
        case TrackElemType::RightTwistUpToDown:
            return InvertedRCTrackRightTwistUpToDown;
        case TrackElemType::HalfLoopUp:
            return InvertedRCTrackHalfLoopUp;
        case TrackElemType::HalfLoopDown:
            return InvertedRCTrackHalfLoopDown;
        case TrackElemType::LeftCorkscrewUp:
            return InvertedRCTrackLeftCorkscrewUp;
        case TrackElemType::RightCorkscrewUp:
            return InvertedRCTrackRightCorkscrewUp;
        case TrackElemType::LeftCorkscrewDown:
            return InvertedRCTrackLeftCorkscrewDown;
        case TrackElemType::RightCorkscrewDown:
            return InvertedRCTrackRightCorkscrewDown;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return InvertedRCTrackLeftQuarterTurn160DegUp;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return InvertedRCTrackRightQuarterTurn160DegUp;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return InvertedRCTrackLeftQuarterTurn160DegDown;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return InvertedRCTrackRightQuarterTurn160DegDown;
        case TrackElemType::Brakes:
            return InvertedRCTrackBrakes;
        case TrackElemType::LeftQuarterBankedHelixLargeUp:
            return InvertedRCTrackLeftQuarterBankedHelixLargeUp;
        case TrackElemType::RightQuarterBankedHelixLargeUp:
            return InvertedRCTrackRightQuarterBankedHelixLargeUp;
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
            return InvertedRCTrackLeftQuarterBankedHelixLargeDown;
        case TrackElemType::RightQuarterBankedHelixLargeDown:
            return InvertedRCTrackRightQuarterBankedHelixLargeDown;
        case TrackElemType::Up25LeftBanked:
            return InvertedRCTrack25DegUpLeftBanked;
        case TrackElemType::Up25RightBanked:
            return InvertedRCTrack25DegUpRightBanked;
        case TrackElemType::OnRidePhoto:
            return InvertedRCTrackOnRidePhoto;
        case TrackElemType::Down25LeftBanked:
            return InvertedRCTrack25DegDownLeftBanked;
        case TrackElemType::Down25RightBanked:
            return InvertedRCTrack25DegDownRightBanked;
        case TrackElemType::LeftEighthToDiag:
            return InvertedRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return InvertedRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return InvertedRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return InvertedRCTrackRightEighthToOrthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return InvertedRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return InvertedRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return InvertedRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return InvertedRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlat:
            return InvertedRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return InvertedRCTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return InvertedRCTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return InvertedRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return InvertedRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return InvertedRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return InvertedRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return InvertedRCTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return InvertedRCTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return InvertedRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return InvertedRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return InvertedRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return InvertedRCTrackDiag25DegDownToFlat;
        case TrackElemType::DiagFlatToLeftBank:
            return InvertedRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return InvertedRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return InvertedRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return InvertedRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return InvertedRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return InvertedRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return InvertedRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return InvertedRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return InvertedRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return InvertedRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return InvertedRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return InvertedRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return InvertedRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return InvertedRCTrackDiagRightBank;
        case TrackElemType::LeftLargeHalfLoopUp:
            return InvertedRCTrackLeftLargeHalfLoopUp;
        case TrackElemType::RightLargeHalfLoopUp:
            return InvertedRCTrackRightLargeHalfLoopUp;
        case TrackElemType::RightLargeHalfLoopDown:
            return InvertedRCTrackRightLargeHalfLoopDown;
        case TrackElemType::LeftLargeHalfLoopDown:
            return InvertedRCTrackLeftLargeHalfLoopDown;
        case TrackElemType::BlockBrakes:
            return InvertedRCTrackBlockBrakes;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
            return InvertedRCTrackLeftBankedQuarterTurn325DegUp;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
            return InvertedRCTrackRightBankedQuarterTurn325DegUp;
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return InvertedRCTrackLeftBankedQuarterTurn325DegDown;
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return InvertedRCTrackRightBankedQuarterTurn325DegDown;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
            return InvertedRCTrackLeftBankedQuarterTurn525DegUp;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
            return InvertedRCTrackRightBankedQuarterTurn525DegUp;
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return InvertedRCTrackLeftBankedQuarterTurn525DegDown;
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return InvertedRCTrackRightBankedQuarterTurn525DegDown;
        case TrackElemType::Up25ToLeftBankedUp25:
            return InvertedRCTrack25DegUpToLeftBanked25DegUp;
        case TrackElemType::Up25ToRightBankedUp25:
            return InvertedRCTrack25DegUpToRightBanked25DegUp;
        case TrackElemType::LeftBankedUp25ToUp25:
            return InvertedRCTrackLeftBanked25DegUpTo25DegUp;
        case TrackElemType::RightBankedUp25ToUp25:
            return InvertedRCTrackRightBanked25DegUpTo25DegUp;
        case TrackElemType::Down25ToLeftBankedDown25:
            return InvertedRCTrack25DegDownToLeftBanked25DegDown;
        case TrackElemType::Down25ToRightBankedDown25:
            return InvertedRCTrack25DegDownToRightBanked25DegDown;
        case TrackElemType::LeftBankedDown25ToDown25:
            return InvertedRCTrackLeftBanked25DegDownTo25DegDown;
        case TrackElemType::RightBankedDown25ToDown25:
            return InvertedRCTrackRightBanked25DegDownTo25DegDown;
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
            return InvertedRCTrackLeftBankedFlatToLeftBanked25DegUp;
        case TrackElemType::RightBankedFlatToRightBankedUp25:
            return InvertedRCTrackRightBankedFlatToRightBanked25DegUp;
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
            return InvertedRCTrackLeftBanked25DegUpToLeftBankedFlat;
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
            return InvertedRCTrackRightBanked25DegUpToRightBankedFlat;
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
            return InvertedRCTrackLeftBankedFlatToLeftBanked25DegDown;
        case TrackElemType::RightBankedFlatToRightBankedDown25:
            return InvertedRCTrackRightBankedFlatToRightBanked25DegDown;
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
            return InvertedRCTrackLeftBanked25DegDownToLeftBankedFlat;
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
            return InvertedRCTrackRightBanked25DegDownToRightBankedFlat;
        case TrackElemType::FlatToLeftBankedUp25:
            return InvertedRCTrackFlatToLeftBanked25DegUp;
        case TrackElemType::FlatToRightBankedUp25:
            return InvertedRCTrackFlatToRightBanked25DegUp;
        case TrackElemType::LeftBankedUp25ToFlat:
            return InvertedRCTrackLeftBanked25DegUpToFlat;
        case TrackElemType::RightBankedUp25ToFlat:
            return InvertedRCTrackRightBanked25DegUpToFlat;
        case TrackElemType::FlatToLeftBankedDown25:
            return InvertedRCTrackFlatToLeftBanked25DegDown;
        case TrackElemType::FlatToRightBankedDown25:
            return InvertedRCTrackFlatToRightBanked25DegDown;
        case TrackElemType::LeftBankedDown25ToFlat:
            return InvertedRCTrackLeftBanked25DegDownToFlat;
        case TrackElemType::RightBankedDown25ToFlat:
            return InvertedRCTrackRightBanked25DegDownToFlat;

        case TrackElemType::Booster:
            return InvertedRCTrackBooster;
    }
    return nullptr;
}
