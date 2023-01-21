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
#include "BolligerMabillardTrack.hpp"

/** rct2: 0x007C6FF4 */
static void FlyingRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17486), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17487), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17488), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17489), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17146), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17147), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27131), { 0, 0, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 22 });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27132), { 0, 0, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 22 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27129), { 0, 0, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 22 });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27130), { 0, 0, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 22 });
                    break;
            }
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C7244, 0x007C7254, 0x007C7264 */
static void FlyingRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { SPR_STATION_BASE_C_SW_NE, 27131, SPR_STATION_INVERTED_BAR_C_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 27132, SPR_STATION_INVERTED_BAR_C_NW_SE },
            { SPR_STATION_BASE_C_SW_NE, 27131, SPR_STATION_INVERTED_BAR_C_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 27132, SPR_STATION_INVERTED_BAR_C_NW_SE },
        };
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][0]), { 0, 0, height },
            { 32, 28, 1 }, { 0, 2, height });
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]), { 0, 0, height + 24 },
            { 32, 20, 3 }, { 0, 6, height + 24 });
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours[SCHEME_SUPPORTS].WithIndex(imageIds[direction][2]), { 0, 6, height + 24 },
            { 32, 20, 1 }, { 0, 6, height + 24 });
        TrackPaintUtilDrawStationMetalSupports2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 11);
        TrackPaintUtilDrawStationInverted(session, ride, direction, height, trackElement, STATION_VARIANT_1);
    }
    else
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { 17154, 17150, SPR_STATION_BASE_A_SW_NE },
            { 17155, 17151, SPR_STATION_BASE_A_NW_SE },
            { 17154, 17150, SPR_STATION_BASE_A_SW_NE },
            { 17155, 17151, SPR_STATION_BASE_A_NW_SE },
        };

        if (trackElement.GetTrackType() == TrackElemType::EndStation)
        {
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]), { 0, 0, height },
                { 32, 20, 1 }, { 0, 6, height + 3 });
        }
        else
        {
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]), { 0, 0, height },
                { 32, 20, 1 }, { 0, 6, height + 3 });
        }
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][2]), { 0, 0, height },
            { 32, 32, 1 });
        TrackPaintUtilDrawStationMetalSupports2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 11);
        TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, 9, 11);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x007C7004 */
static void FlyingRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17498), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17499), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17500), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17501), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17204), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17205), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17206), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17207), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27249), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 40 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27250), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 40 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27251), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 40 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27252), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 40 });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27221), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 40 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27222), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 40 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27223), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 40 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27224), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
    }
}

/** rct2: 0x007C7014 */
static void FlyingRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17220), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17221), { 0, 0, height }, { 32, 1, 98 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17222), { 0, 0, height }, { 32, 1, 98 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17223), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27237), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 88 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27238), { 0, 0, height + 24 },
                    { 32, 2, 81 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27239), { 0, 0, height + 24 },
                    { 32, 2, 81 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27240), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 88 });
                break;
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
}

/** rct2: 0x007C7024 */
static void FlyingRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17490), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17491), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17492), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17493), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17196), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17197), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17198), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17199), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27241), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27242), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27243), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27244), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27213), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27214), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27215), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27216), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }
}

/** rct2: 0x007C7034 */
static void FlyingRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17208), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17209), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17212), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17210), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17213), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17211), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27225), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27229), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27226), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27230), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27227), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27228), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
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
}

/** rct2: 0x007C7044 */
static void FlyingRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17214), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17215), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17218), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17216), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17219), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17217), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27231), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27235), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27232), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27236), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27233), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27234), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 71, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 71, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 71, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 71, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7054 */
static void FlyingRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17494), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17495), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17496), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17497), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17200), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17201), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17202), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17203), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27245), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27246), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27247), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27248), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27217), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27218), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27219), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27220), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
    }
}

/** rct2: 0x007C7064 */
static void FlyingRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7074 */
static void FlyingRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7084 */
static void FlyingRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7094 */
static void FlyingRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C70A4 */
static void FlyingRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C70B4 */
static void FlyingRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C70C4 */
static void FlyingRCTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17259), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17264), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17269), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17254), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17258), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17263), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17268), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17253), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17257), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17262), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17267), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17252), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17256), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17261), { 0, 0, height },
                            { 16, 32, 3 }, { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17266), { 0, 0, height },
                            { 16, 32, 3 }, { 0, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17251), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17255), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17260), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17265), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17250), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27142), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27147), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27152), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27137), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27141), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27146), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27151), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27136), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27140), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27145), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27150), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27135), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27139), { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27144), { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27149), { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27134), { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 16, 0, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27138), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27143), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27148), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27133), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C70D4 */
static void FlyingRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    FlyingRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C70E4 */
static void FlyingRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17156), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17164), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17157), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17165), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17158), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17159), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27269), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27270), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27271), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27272), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C70F4 */
static void FlyingRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17160), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17161), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17162), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17166), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17163), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17167), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27273), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27274), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27275), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27276), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C7104 */
static void FlyingRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17162), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17166), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17163), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17167), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17160), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17161), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27275), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27276), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27273), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27274), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C7114 */
static void FlyingRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17158), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17159), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17156), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17164), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17157), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17165), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27271), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27272), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27269), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27270), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C7124 */
static void FlyingRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17279), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17290), { 0, 0, height },
                            { 32, 1, 26 }, { 0, 27, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17284), { 0, 0, height },
                            { 32, 1, 26 }, { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17289), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17274), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17278), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17283), { 0, 0, height },
                            { 32, 16, 1 }, { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17288), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17273), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17277), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17282), { 0, 0, height },
                            { 16, 16, 1 }, { 16, 16, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17287), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17272), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17276), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17281), { 0, 0, height },
                            { 16, 32, 1 }, { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17286), { 0, 0, height },
                            { 16, 32, 3 }, { 0, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17271), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17275), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17280), { 0, 0, height },
                            { 1, 32, 26 }, { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17285), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17291), { 0, 0, height },
                            { 1, 32, 26 }, { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17270), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27162), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27167), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27172), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27157), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27161), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27166), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27171), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27156), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 24 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27160), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27165), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27170), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27155), { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27159), { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 16, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27164), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27169), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27154), { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 16, 0, height + 24 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27158), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27163), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27168), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27153), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7134 */
static void FlyingRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    FlyingRCTrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7144 */
static void FlyingRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17168), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17172), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17169), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17173), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17170), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17171), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27277), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27278), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27279), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27280), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }
}

/** rct2: 0x007C7154 */
static void FlyingRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17174), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17175), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17176), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17178), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17177), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17179), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27281), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27282), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27283), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27284), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }
}

/** rct2: 0x007C7164 */
static void FlyingRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17180), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17184), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17181), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17185), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17182), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17183), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27285), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27286), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27287), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27288), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
    }
}

/** rct2: 0x007C7174 */
static void FlyingRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17186), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17187), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17188), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17190), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17189), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17191), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27289), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27290), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27291), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27292), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
    }
}

/** rct2: 0x007C7184 */
static void FlyingRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7194 */
static void FlyingRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C71A4 */
static void FlyingRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C71B4 */
static void FlyingRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C71C4 */
static void FlyingRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17192), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17193), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17194), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17195), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27293), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27294), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27295), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27296), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 22 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C71D4 */
static void FlyingRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C71E4 */
static void FlyingRCTrackLeftQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17344), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17349), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17354), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17359), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17345), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17350), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17355), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17360), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17346), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17351), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17356), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17361), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17347), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17352), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17357), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17362), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17348), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17353), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17358), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17363), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27317), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27322), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27327), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27332), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
                }
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27318), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27323), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27328), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27333), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27319), { 0, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27324), { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27329), { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27334), { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27320), { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27325), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27330), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27335), { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27321), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27326), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27331), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27336), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C71F4 */
static void FlyingRCTrackRightQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17324), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17329), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17334), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17339), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17325), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17330), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17335), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17340), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17326), { 0, 0, height },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17331), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17336), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17341), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17327), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17332), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17337), { 0, 0, height },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17342), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17328), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17333), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17338), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17343), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27297), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27302), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27307), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27312), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
                }
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27298), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27303), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27308), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27313), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27299), { 0, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27304), { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27309), { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27314), { 0, 16, height + 24 },
                            { 16, 16, 3 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27300), { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27305), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27310), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27315), { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27301), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27306), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27311), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27316), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7204 */
static void FlyingRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    FlyingRCTrackRightQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C7214 */
static void FlyingRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    FlyingRCTrackLeftQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7224 */
static void FlyingRCTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17308), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17312), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17311), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17315), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17309), { 0, 0, height },
                            { 32, 26, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17313), { 0, 0, height },
                            { 32, 26, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17310), { 0, 0, height },
                            { 32, 26, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17314), { 0, 0, height },
                            { 32, 26, 3 }, { 0, 6, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17310), { 0, 0, height },
                            { 32, 26, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17314), { 0, 0, height },
                            { 32, 26, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17309), { 0, 0, height },
                            { 32, 26, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17313), { 0, 0, height },
                            { 32, 26, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17311), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17315), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17308), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17312), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27253), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27257), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27256), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27260), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27254), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27258), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27255), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27259), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27255), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27259), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27254), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27258), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27256), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27260), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27253), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27257), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7234 */
static void FlyingRCTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17316), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17320), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17319), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17323), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17317), { 0, 0, height },
                            { 32, 26, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17321), { 0, 0, height },
                            { 32, 26, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17318), { 0, 0, height },
                            { 32, 26, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17322), { 0, 0, height },
                            { 32, 26, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17318), { 0, 0, height },
                            { 32, 26, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17322), { 0, 0, height },
                            { 32, 26, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17317), { 0, 0, height },
                            { 32, 26, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17321), { 0, 0, height },
                            { 32, 26, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17319), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17323), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17316), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17320), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27261), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27265), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27264), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27268), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27262), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27266), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27263), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27267), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27263), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27267), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27262), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27266), { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27264), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27268), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27261), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27265), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7274 */
static void FlyingRCTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17229), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17232), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17235), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17226), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17228), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17231), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17234), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17225), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17227), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17230), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17233), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17224), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27392), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27395), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27398), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27389), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27391), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27394), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27397), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27388), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27390), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27393), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27396), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27387), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7284 */
static void FlyingRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    FlyingRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7294 */
static void FlyingRCTrackLeftQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17241), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17248), { 0, 0, height },
                            { 32, 1, 26 }, { 0, 27, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17244), { 0, 0, height },
                            { 32, 1, 26 }, { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17247), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17238), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17240), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17243), { 0, 0, height },
                            { 16, 16, 1 }, { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17246), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17237), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17239), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17242), { 0, 0, height },
                            { 1, 32, 26 }, { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17245), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17249), { 0, 0, height },
                            { 1, 32, 26 }, { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17236), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27404), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27407), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27410), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27401), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27403), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27406), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27409), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27400), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27402), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27405), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27408), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27399), { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C72A4 */
static void FlyingRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    FlyingRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C72B4 */
static void FlyingRCTrackLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17375), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17377), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17379), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17373), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17374), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17376), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17378), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17372), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27422), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27424), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27426), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27420), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
                }
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27421), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27423), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27425), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27419), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C72C4 */
static void FlyingRCTrackRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17364), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17366), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17368), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17370), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17365), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17367), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17369), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17371), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27411), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27413), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27415), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27417), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
                }
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27412), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27414), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27416), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27418), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C72D4 */
static void FlyingRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    FlyingRCTrackRightQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C72E4 */
static void FlyingRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    FlyingRCTrackLeftQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7314 */
static void FlyingRCTrackLeftHalfBankedHelixUpSmall(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17399), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17406), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17402), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17405), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17396), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17398), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17401), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17404), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17395), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17397), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17400), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17403), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17407), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17394), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17396), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17399), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17406), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17402), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17405), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17395), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17398), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17401), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17404), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17394), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17397), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17400), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17403), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17407), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x007C7324 */
static void FlyingRCTrackRightHalfBankedHelixUpSmall(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17380), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17383), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17386), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17389), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17393), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17381), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17384), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17387), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17390), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17382), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17385), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17392), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17388), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17391), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17383), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17386), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17389), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17393), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17380), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17384), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17387), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17390), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17381), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17385), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17392), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17388), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17391), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17382), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x007C7334 */
static void FlyingRCTrackLeftHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    FlyingRCTrackRightHalfBankedHelixUpSmall(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C7344 */
static void FlyingRCTrackRightHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    FlyingRCTrackLeftHalfBankedHelixUpSmall(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7354 */
static void FlyingRCTrackLeftHalfBankedHelixUpLarge(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17439), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17450), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17444), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17449), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17434), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17438), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17443), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17448), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17433), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17437), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17442), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17447), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17432), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17436), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17441), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17446), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17431), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17435), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17440), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17445), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17451), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17430), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17434), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17439), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17450), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17444), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17449), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17433), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17438), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17443), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17448), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17432), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17437), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17442), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17447), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17431), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17436), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17441), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17446), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17430), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17435), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17440), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17445), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17451), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x007C7364 */
static void FlyingRCTrackRightHalfBankedHelixUpLarge(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17408), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17413), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17418), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17423), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17429), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17409), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17414), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17419), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17424), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17410), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17415), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17420), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17425), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17411), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17416), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17421), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17426), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17412), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17417), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17428), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17422), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17427), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17413), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17418), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17423), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17429), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17408), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17414), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17419), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17424), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17409), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17415), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17420), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17425), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17410), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17416), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17421), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17426), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17411), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17417), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17428), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17422), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17427), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17412), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x007C7374 */
static void FlyingRCTrackLeftHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    FlyingRCTrackRightHalfBankedHelixUpLarge(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C7384 */
static void FlyingRCTrackRightHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    FlyingRCTrackLeftHalfBankedHelixUpLarge(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C73B4 */
static void FlyingRCTrackLeftQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17301), { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17305), { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17302), { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17306), { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17303), { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17307), { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17300), { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17304), { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27352), { 0, 0, height - 5 },
                    { 28, 28, 3 }, { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27348), { 0, 0, height - 5 },
                    { 28, 28, 1 }, { 2, 2, height + 94 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27353), { 0, 0, height - 5 },
                    { 28, 28, 3 }, { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27349), { 0, 0, height - 5 },
                    { 28, 28, 1 }, { 2, 2, height + 94 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27354), { 0, 0, height - 5 },
                    { 28, 28, 3 }, { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27350), { 0, 0, height - 5 },
                    { 28, 28, 1 }, { 2, 2, height + 94 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27351), { 0, 0, height - 5 },
                    { 28, 28, 3 }, { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27347), { 0, 0, height - 5 },
                    { 28, 28, 1 }, { 2, 2, height + 94 });
                break;
        }
    }

    TrackPaintUtilLeftQuarterTurn1TileTunnel(session, direction, height, -8, TUNNEL_SQUARE_7, +56, TUNNEL_SQUARE_8);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x007C7394 */
static void FlyingRCTrackRightQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17292), { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17296), { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17293), { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17297), { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17294), { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17298), { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17295), { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17299), { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27343), { 0, 0, height - 5 },
                    { 28, 28, 3 }, { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27339), { 0, 0, height - 5 },
                    { 28, 28, 1 }, { 2, 2, height + 94 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27344), { 0, 0, height - 5 },
                    { 28, 28, 3 }, { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27340), { 0, 0, height - 5 },
                    { 28, 28, 1 }, { 2, 2, height + 94 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27345), { 0, 0, height - 5 },
                    { 28, 28, 3 }, { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27341), { 0, 0, height - 5 },
                    { 28, 28, 1 }, { 2, 2, height + 94 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27346), { 0, 0, height - 5 },
                    { 28, 28, 3 }, { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27342), { 0, 0, height - 5 },
                    { 28, 28, 1 }, { 2, 2, height + 94 });
                break;
        }
    }
    TrackPaintUtilRightQuarterTurn1TileTunnel(session, direction, height, -8, TUNNEL_SQUARE_7, +56, TUNNEL_SQUARE_8);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x007C73A4 */
static void FlyingRCTrackLeftQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackRightQuarterTurn160DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C73C4 */
static void FlyingRCTrackRightQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackLeftQuarterTurn160DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C73D4 */
static void FlyingRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17148), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17149), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27337), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 24 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27338), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 24 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

static void FlyingRCTrackBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        BolligerMabillardTrackBooster<METAL_SUPPORTS_TUBES_INVERTED>(
            session, ride, trackSequence, direction, height, trackElement);
    }
    else
    {
        const auto imageId = (direction & 1) ? SPR_G2_BM_INVERT_BOOSTER_2 : SPR_G2_BM_INVERT_BOOSTER_1;
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageId), { 0, 0, height + 24 }, { 32, 20, 1 },
            { 0, 6, height + 22 });

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C7674 */
static void FlyingRCTrackLeftQuarterBankedHelixLargeUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27202), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27207), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27212), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27197), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 30 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27201), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27206), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27211), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27196), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 16, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27200), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27205), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 16, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27210), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 16, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27195), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 0, 0, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27199), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27204), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27209), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27194), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 16, 0, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27198), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27203), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27208), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27193), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 38 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 53, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 16, TUNNEL_SQUARE_INVERTED_9);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 16, TUNNEL_SQUARE_INVERTED_9);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x007C7684 */
static void FlyingRCTrackRightQuarterBankedHelixLargeUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27173), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27178), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27183), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27188), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 30 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27174), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27179), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27184), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27189), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 0, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27175), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27180), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 16, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27185), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 16, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27190), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 0, 16, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27176), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27181), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27186), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27191), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 16, 0, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27177), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27182), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27187), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27192), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 38 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 53, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 16, TUNNEL_SQUARE_INVERTED_9);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 16, TUNNEL_SQUARE_INVERTED_9);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x007C7694 */
static void FlyingRCTrackLeftQuarterBankedHelixLargeDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27182), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27187), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27192), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27177), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 38 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 53, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, TUNNEL_SQUARE_INVERTED_9);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27181), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27186), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27191), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27176), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 16, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27180), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27185), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 16, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27190), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 16, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27175), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 0, 0, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27179), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27184), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27189), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27174), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 16, 0, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27178), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27183), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27188), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27173), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 30 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_INVERTED_9);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_INVERTED_9);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x007C76A4 */
static void FlyingRCTrackRightQuarterBankedHelixLargeDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27193), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27198), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27203), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27208), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 38 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 53, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, TUNNEL_SQUARE_INVERTED_9);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27194), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27199), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27204), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27209), { 0, 0, height + 24 },
                        { 32, 16, 3 }, { 0, 0, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27195), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27200), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 16, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27205), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 16, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27210), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 0, 16, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27196), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27201), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27206), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27211), { 0, 0, height + 24 },
                        { 16, 32, 3 }, { 16, 0, height + 38 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27197), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27202), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27207), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27212), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 30 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_INVERTED_9);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_INVERTED_9);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x007C78B4 */
static void FlyingRCTrack25DegUpLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17914), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17915), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17916), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17917), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27711), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27712), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27713), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27714), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C78C4 */
static void FlyingRCTrack25DegUpRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17918), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17919), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17920), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17921), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27715), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27716), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27717), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27718), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C73E4 */
static void FlyingRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17146), { 0, 0, height }, { 32, 20, 0 },
                    { 0, 6, height + 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17147), { 0, 0, height }, { 32, 20, 0 },
                    { 0, 6, height + 3 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17146), { 0, 0, height }, { 32, 20, 0 },
                    { 0, 6, height + 3 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17147), { 0, 0, height }, { 32, 20, 0 },
                    { 0, 6, height + 3 });
                break;
        }
        TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27129), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 24 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27130), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 24 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27129), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 24 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27130), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 24 });
                break;
        }
        TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
    }
}

/** rct2: 0x007C78D4 */
static void FlyingRCTrack25DegDownLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack25DegUpRightBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C78E4 */
static void FlyingRCTrack25DegDownRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack25DegUpLeftBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7404 */
static void FlyingRCTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17546), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17550), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17554), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17558), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17547), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17551), { 0, 0, height },
                            { 34, 16, 3 }, { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17555), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17559), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17548), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17552), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17556), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17560), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17549), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17553), { 0, 0, height },
                            { 16, 18, 3 }, { 0, 16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17557), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17561), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27507), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27511), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27515), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27519), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27508), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27512), { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27516), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27520), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27509), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27513), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27517), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27521), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27510), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27514), { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27518), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27522), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7414 */
static void FlyingRCTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17530), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17534), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17538), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17542), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17531), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17535), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17539), { 0, 0, height },
                            { 34, 16, 3 }, { 0, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17543), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17532), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17536), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17540), { 0, 0, height },
                            { 28, 28, 3 }, { 4, 4, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17544), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17533), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17537), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17541), { 0, 0, height },
                            { 16, 18, 3 }, { 0, 16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17545), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27491), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27495), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27499), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27503), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27492), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27496), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27500), { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27504), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27493), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27497), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27501), { 0, 0, height + 24 },
                            { 28, 28, 3 }, { 4, 4, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27505), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27494), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27498), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27502), { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27506), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7424 */
static void FlyingRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    FlyingRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7434 */
static void FlyingRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    FlyingRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x007C7444 */
static void FlyingRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17578), { 0, 0, height },
                            { 32, 1, 26 }, { 0, 27, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17582), { 0, 0, height },
                            { 32, 1, 26 }, { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17586), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17590), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17579), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17583), { 0, 0, height },
                            { 34, 16, 0 }, { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17587), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17591), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17580), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17584), { 0, 0, height },
                            { 16, 16, 0 }, { 16, 16, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17588), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17592), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17581), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17585), { 0, 0, height },
                            { 16, 18, 0 }, { 0, 16, height + 27 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17589), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17593), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27539), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27543), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27547), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27551), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27540), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27544), { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27548), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27552), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27541), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27545), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27549), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27553), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27542), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27546), { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27550), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27554), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7454 */
static void FlyingRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17562), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17566), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17570), { 0, 0, height },
                            { 32, 1, 26 }, { 0, 27, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17574), { 0, 0, height },
                            { 32, 1, 26 }, { 0, 27, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17563), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17567), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17571), { 0, 0, height },
                            { 34, 16, 0 }, { 0, 0, height + 27 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17575), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17564), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17568), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17572), { 0, 0, height },
                            { 28, 28, 0 }, { 4, 4, height + 27 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17576), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17565), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17569), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17573), { 0, 0, height },
                            { 16, 18, 0 }, { 0, 16, height + 27 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17577), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27523), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27527), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27531), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27535), { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27524), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27528), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27532), { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27536), { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27525), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27529), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27533), { 0, 0, height + 24 },
                            { 28, 28, 3 }, { 4, 4, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27537), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27526), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27530), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27534), { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27538), { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7464 */
static void FlyingRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    FlyingRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7474 */
static void FlyingRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    FlyingRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x007C73F4 */
static void FlyingRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17861), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17791), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17858), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17788), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17860), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17790), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17859), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17789), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27614),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27558),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 22 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27611),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27555),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 22 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27613),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27557),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 22 });
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
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27612),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27556),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74A4 */
static void FlyingRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17873), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17803), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17870), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17800), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17872), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17802), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17871), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17801), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
    else
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27626),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27570),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 40 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27623),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27567),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 40 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27625),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27569),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 40 });
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
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27624),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27568),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7504 */
static void FlyingRCTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17885), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17815), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17882), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17812), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17884), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17814), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17883), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17813), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27582), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27579), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27581), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27580), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 24 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 32, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 36, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 32, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 36, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7484 */
static void FlyingRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17865), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17795), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17862), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17792), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17864), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17794), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17863), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17793), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
    else
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27618),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27562),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27615),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27559),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27617),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27561),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27616),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27560),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74E4 */
static void FlyingRCTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17877), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17807), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17874), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17804), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17876), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17806), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17875), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17805), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27574), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27571), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27573), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27572), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 16, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 16, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 16, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 16, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74F4 */
static void FlyingRCTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17881), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17811), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17878), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17808), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17880), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17810), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17879), { -16, -16, height },
                                { 16, 16, 3 }, { 0, 0, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17809), { -16, -16, height },
                                { 16, 16, 3 }, { 0, 0, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27578), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27575), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27577), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27576), { -16, -16, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 56 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 21, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 21, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 21, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 21, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7494 */
static void FlyingRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17869), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17799), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17866), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17796), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17868), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17798), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17867), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17797), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
    else
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27622),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27566),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27619),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27563),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27621),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27565),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
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
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27620),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27564),
                                { -16, -16, height + 24 }, { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74D4 */
static void FlyingRCTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17871), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17801), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17872), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17802), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17870), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17800), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17873), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17803), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27568), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27569), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27567), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27570), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7534 */
static void FlyingRCTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17883), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17813), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17884), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17814), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17882), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17812), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17885), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17815), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27580), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 24 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27581), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27579), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27582), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 24, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 28, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 24, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 28, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74B4 */
static void FlyingRCTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17867), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17797), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17868), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17798), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17866), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17796), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17869), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17799), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27564), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27565), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27563), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27566), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                break;
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x007C7514 */
static void FlyingRCTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17879), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17809), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17880), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17810), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17878), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17808), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17881), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17811), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27576), { -16, -16, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27577), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27575), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27578), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 17, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 17, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 17, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 17, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7524 */
static void FlyingRCTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17875), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17805), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17876), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17806), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17874), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17804), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17877), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17807), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27572), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27573), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27571), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27574), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74C4 */
static void FlyingRCTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17863), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17793), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17864), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17794), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17862), { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17792), { -16, -16, height },
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
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17865), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17795), { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27560), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27561), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27559), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27562), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7564 */
static void FlyingRCTrackDiagFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17831), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17828), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17832), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17830), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17829), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27590), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27587), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27589), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27588), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7574 */
static void FlyingRCTrackDiagFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17836), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17833), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17835), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17837), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17834), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27594), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27591), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27593), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27592), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7584 */
static void FlyingRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17834), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17835), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17837), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17833), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17836), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27592), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27593), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27591), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27594), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7594 */
static void FlyingRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17829), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17830), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17828), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17832), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17831), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27588), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27589), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27587), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27590), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C75C4 */
static void FlyingRCTrackDiagLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17851), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17848), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17852), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17850), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17849), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27606), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27603), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27605), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27604), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C75D4 */
static void FlyingRCTrackDiagRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17856), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17853), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17855), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17857), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17854), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27610), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27607), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27609), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27608), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C75A4 */
static void FlyingRCTrackDiag25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17841), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17838), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17842), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17840), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17839), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27598), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27595), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27597), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27596), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C75B4 */
static void FlyingRCTrackDiag25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17846), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17843), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17845), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17847), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17844), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27602), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27599), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27601), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27600), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C75E4 */
static void FlyingRCTrackDiagLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17844), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17845), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17847), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17843), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17846), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27600), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27601), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27599), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27602), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                break;
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x007C75F4 */
static void FlyingRCTrackDiagRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17839), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17840), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17838), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17842), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17841), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27596), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27597), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27595), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27598), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                break;
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x007C7604 */
static void FlyingRCTrackDiag25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17854), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17855), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17857), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17853), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17856), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27608), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27609), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27607), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27610), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7614 */
static void FlyingRCTrackDiag25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17849), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17850), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17848), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17852), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17851), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27604), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27605), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27603), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27606), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7544 */
static void FlyingRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17827), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17824), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17826), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17825), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27586), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27583), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27585), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27584), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7554 */
static void FlyingRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17825), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17826), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17824), { -16, -16, height },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17827), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27584), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27585), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27583), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27586), { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7624 */
static void FlyingRCTrackLeftFlyerTwistUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27430), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27436), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27429), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27435), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height - 5, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27431), { 0, 6, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27437), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27428), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27434), { 0, 6, height + 24 },
                        { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27432), { 0, 6, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27438), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27427), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27433), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x007C7634 */
static void FlyingRCTrackRightFlyerTwistUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27442), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27448), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27441), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27447), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height - 5, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27443), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27449), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27440), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27446), { 0, 6, height + 24 },
                        { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27444), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27450), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27439), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27445), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x007C7644 */
static void FlyingRCTrackLeftFlyerTwistDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27427), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27433), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27432), { 0, 6, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27438), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27428), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27434), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27431), { 0, 6, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27437), { 0, 6, height + 24 },
                        { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27429), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27435), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27430), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27436), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height - 5, session.TrackColours[SCHEME_SUPPORTS]);
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
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x007C7654 */
static void FlyingRCTrackRightFlyerTwistDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27439), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27445), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27444), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27450), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27440), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27446), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27443), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27449), { 0, 6, height + 24 },
                        { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27441), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27447), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27442), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27448), { 0, 6, height - 5 },
                        { 32, 20, 3 }, { 0, 6, height + 6 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height - 5, session.TrackColours[SCHEME_SUPPORTS]);
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
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x007C72F4 */
static void FlyingRCTrackFlyerHalfLoopUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17626), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17634), { 0, 6, height },
                        { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17633), { 0, 6, height },
                        { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17641), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17627), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17635), { 0, 14, height },
                        { 3, 20, 63 }, { 28, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17632), { 0, 6, height },
                        { 3, 20, 63 }, { 28, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17640), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17628), { 16, 0, height },
                        { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17636), { 12, 0, height },
                        { 3, 16, 119 }, { 12, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17631), { 10, 16, height },
                        { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17639), { 16, 16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17629), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17637), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17630), { 0, 16, height + 32 },
                        { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17638), { 0, 16, height + 32 },
                        { 32, 12, 3 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x007C7304 */
static void FlyingRCTrackFlyerHalfLoopDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17629), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17637), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17630), { 0, 16, height + 32 },
                        { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17638), { 0, 16, height + 32 },
                        { 32, 12, 3 });
                    break;
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
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17628), { 16, 0, height },
                        { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17636), { 12, 0, height },
                        { 3, 16, 119 }, { 12, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17631), { 10, 16, height },
                        { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17639), { 16, 16, height },
                        { 2, 16, 119 }, { 15, 6, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17627), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17635), { 0, 14, height },
                        { 3, 20, 63 }, { 28, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17632), { 0, 6, height },
                        { 3, 20, 63 }, { 28, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17640), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17626), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17634), { 0, 6, height },
                        { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17633), { 0, 6, height },
                        { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17641), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x007C7664 */
static void FlyingRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17150), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17151), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27337), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 24 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27338), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 24 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C76B4 */
static void FlyingRCTrackLeftBankedQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18025), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18027), { 0, 6, height },
                            { 32, 1, 34 }, { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18029), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18023), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18024), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18026), { 6, 0, height },
                            { 1, 32, 34 }, { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18028), { 6, 0, height },
                            { 1, 32, 34 }, { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18022), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27762), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27764), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27766), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27760), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27761), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27763), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27765), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27759), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C76C4 */
static void FlyingRCTrackRightBankedQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18014), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18016), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18018), { 0, 6, height },
                            { 32, 1, 34 }, { 0, 27, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18020), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18015), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18017), { 6, 0, height },
                            { 1, 32, 34 }, { 27, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18019), { 6, 0, height },
                            { 1, 32, 34 }, { 27, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18021), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27751), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27753), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27755), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27757), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27752), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27754), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27756), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27758), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C76D4 */
static void FlyingRCTrackLeftBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    FlyingRCTrackRightBankedQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C76E4 */
static void FlyingRCTrackRightBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    FlyingRCTrackLeftBankedQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C76F4 */
static void FlyingRCTrackLeftBankedQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17978), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17983), { 0, 0, height },
                            { 32, 1, 34 }, { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17988), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17993), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17979), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17984), { 0, 0, height },
                            { 1, 1, 34 }, { 30, 30, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17989), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17994), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17980), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17985), { 0, 0, height },
                            { 1, 1, 34 }, { 30, 30, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17990), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17995), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17981), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17986), { 0, 0, height },
                            { 1, 1, 34 }, { 30, 30, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17991), { 0, 0, height },
                            { 1, 32, 34 }, { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17996), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17982), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17987), { 0, 0, height },
                            { 1, 32, 34 }, { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17992), { 0, 0, height },
                            { 1, 32, 34 }, { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17997), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27787), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27792), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27797), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27802), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27788), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27793), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27798), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27803), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27789), { 0, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27794), { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27799), { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27804), { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27790), { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27795), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27800), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27805), { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27791), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27796), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27801), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27806), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7704 */
static void FlyingRCTrackRightBankedQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17958), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17963), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17968), { 0, 0, height },
                            { 32, 1, 34 }, { 0, 27, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17973), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17959), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17964), { 0, 0, height },
                            { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17969), { 0, 0, height },
                            { 1, 1, 34 }, { 30, 30, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17974), { 0, 0, height },
                            { 32, 16, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17960), { 0, 0, height },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17965), { 0, 0, height },
                            { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17970), { 0, 0, height },
                            { 1, 1, 34 }, { 30, 30, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17975), { 0, 0, height },
                            { 16, 16, 3 }, { 0, 16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17961), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17966), { 0, 0, height },
                            { 1, 32, 34 }, { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17971), { 0, 0, height },
                            { 1, 1, 34 }, { 30, 30, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17976), { 0, 0, height },
                            { 16, 32, 3 }, { 16, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17962), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17967), { 0, 0, height },
                            { 1, 32, 34 }, { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17972), { 0, 0, height },
                            { 1, 32, 34 }, { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17977), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27767), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27772), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27777), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27782), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27768), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27773), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27778), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27783), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27769), { 0, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27774), { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27779), { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27784), { 0, 16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27770), { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27775), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27780), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27785), { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27771), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27776), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27781), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27786), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7714 */
static void FlyingRCTrackLeftBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    FlyingRCTrackRightBankedQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C7724 */
static void FlyingRCTrackRightBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    FlyingRCTrackLeftBankedQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7734 */
static void FlyingRCTrack25DegUpToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17922), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17923), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17930), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17924), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17925), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27719), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27720), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27721), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27722), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7744 */
static void FlyingRCTrack25DegUpToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17926), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17927), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17928), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17931), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17929), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27723), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27724), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27725), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27726), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7754 */
static void FlyingRCTrackLeftBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17932), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17933), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17940), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17934), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17935), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27727), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27728), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27729), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27730), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7764 */
static void FlyingRCTrackRightBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17936), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17937), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17938), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17941), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17939), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27731), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27732), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27733), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27734), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7774 */
static void FlyingRCTrack25DegDownToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackRightBanked25DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7784 */
static void FlyingRCTrack25DegDownToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackLeftBanked25DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7794 */
static void FlyingRCTrackLeftBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack25DegUpToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C77A4 */
static void FlyingRCTrackRightBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack25DegUpToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C77B4 */
static void FlyingRCTrackLeftBankedFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17942), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17943), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17944), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17945), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27735), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27736), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27737), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27738), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
    }
}

/** rct2: 0x007C77C4 */
static void FlyingRCTrackRightBankedFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17946), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17947), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17948), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17949), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27739), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27740), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27741), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27742), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
    }
}

/** rct2: 0x007C77F4 */
static void FlyingRCTrackLeftBanked25DegUpToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17950), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17951), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17952), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17953), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27743), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27744), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27745), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27746), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
    }
}

/** rct2: 0x007C7804 */
static void FlyingRCTrackRightBanked25DegUpToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17954), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17955), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17956), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17957), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27747), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27748), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27749), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27750), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
    }
}

/** rct2: 0x007C7814 */
static void FlyingRCTrackLeftBankedFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackRightBanked25DegUpToRightBankedFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7824 */
static void FlyingRCTrackRightBankedFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackLeftBanked25DegUpToLeftBankedFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C77D4 */
static void FlyingRCTrackLeftBanked25DegDownToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackRightBankedFlatToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C77E4 */
static void FlyingRCTrackRightBanked25DegDownToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackLeftBankedFlatToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7834 */
static void FlyingRCTrackFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17894), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17895), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17902), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17896), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17897), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27695), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27696), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27697), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27698), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
    }
}

/** rct2: 0x007C7844 */
static void FlyingRCTrackFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17898), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17899), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17900), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17903), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17901), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27699), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27700), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27701), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27702), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
    }
}

/** rct2: 0x007C7854 */
static void FlyingRCTrackLeftBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17904), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17905), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17912), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17906), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17907), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27703), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27704), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27705), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27706), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
    }
}

/** rct2: 0x007C7864 */
static void FlyingRCTrackRightBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17908), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17909), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17910), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17913), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17911), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27707), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27708), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27709), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27710), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
    }
}

/** rct2: 0x007C7874 */
static void FlyingRCTrackFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackRightBanked25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7884 */
static void FlyingRCTrackFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackLeftBanked25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7894 */
static void FlyingRCTrackLeftBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackFlatToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C78A4 */
static void FlyingRCTrackRightBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackFlatToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void FlyingRCTrack90DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
    {
        direction = (direction + 2) & 3;
    }
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17526), { 0, 0, height },
                        { 2, 20, 31 }, { 4, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17527), { 0, 0, height },
                        { 2, 20, 31 }, { 24, 6, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17528), { 0, 0, height },
                        { 2, 20, 31 }, { 24, 6, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17529), { 0, 0, height },
                        { 2, 20, 31 }, { 4, 6, height + 8 });
                    break;
            }
            PaintUtilSetVerticalTunnel(session, height + 32);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            break;
    }
}

static void FlyingRCTrack90DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void FlyingRCTrack60DegUpTo90DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
        return;
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17518), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17519), { 0, 0, height },
                        { 2, 20, 55 }, { 24, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17520), { 0, 0, height },
                        { 2, 20, 55 }, { 24, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17521), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetVerticalTunnel(session, height + 56);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            break;
    }
}

static void FlyingRCTrack90DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack60DegUpTo90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void FlyingRCTrack90DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
        return;
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17522), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height + 8 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17523), { 0, 0, height }, { 2, 20, 31 },
                { 24, 6, height + 8 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17524), { 0, 0, height }, { 2, 20, 31 },
                { 24, 6, height + 8 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17525), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height + 8 });
            break;
    }
    switch (direction)
    {
        case 1:
            PaintUtilPushTunnelRight(session, height + 48, TUNNEL_SQUARE_8);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 48, TUNNEL_SQUARE_8);
            break;
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
}

static void FlyingRCTrack60DegDownTo90DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
        return;
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17524), { 0, 0, height },
                        { 2, 20, 31 }, { 24, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17525), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17522), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17523), { 0, 0, height },
                        { 2, 20, 31 }, { 24, 6, height + 8 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 48, TUNNEL_SQUARE_8);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 1:
            break;
    }
}

static void FlyingRCTrack90DegToInvertedFlatQuarterLoopUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18062), { 0, 0, height },
                        { 2, 20, 31 }, { 4, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18065), { 0, 0, height },
                        { 2, 20, 31 }, { 24, 6, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18068), { 0, 0, height },
                        { 2, 20, 31 }, { 24, 6, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18071), { 0, 0, height },
                        { 2, 20, 31 }, { 4, 6, height + 8 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18063), { 0, 0, height },
                        { 2, 20, 31 }, { -8, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18066), { 0, 0, height },
                        { 2, 20, 63 }, { 24, 6, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18069), { 0, 0, height },
                        { 2, 20, 63 }, { 24, 6, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18072), { 0, 0, height },
                        { 2, 20, 31 }, { -8, 6, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18064), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18067), { 0, 0, height },
                        { 2, 20, 31 }, { 24, 6, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18070), { 0, 0, height },
                        { 2, 20, 31 }, { 24, 6, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18073), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 24 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

static void FlyingRCTrackInvertedFlatTo90DegQuarterLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrack90DegToInvertedFlatQuarterLoopUp(session, ride, 2 - trackSequence, direction, height, trackElement);
}

static void FlyingRCTrackFlatTo60DegUpLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
        return;
    BolligerMabillardTrackFlatTo60DegUpLongBase<METAL_SUPPORTS_TUBES_INVERTED>(
        session, ride, trackSequence, direction, height, trackElement);
}

static void FlyingRCTrack60DegUpToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
        return;
    BolligerMabillardTrack60DegUpToFlatLongBase<METAL_SUPPORTS_TUBES_INVERTED>(
        session, ride, trackSequence, direction, height, trackElement);
}

static void FlyingRCTrack60DegDownToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
        return;
    BolligerMabillardTrackFlatTo60DegUpLongBase<METAL_SUPPORTS_TUBES_INVERTED>(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

static void FlyingRCTrackFlatTo60DegDownLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
        return;
    BolligerMabillardTrack60DegUpToFlatLongBase<METAL_SUPPORTS_TUBES_INVERTED>(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}
static void FlyingRCTrackHalfLoopInvertedUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27451), { 0, 6, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 45 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27459), { 0, 6, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 45 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27458), { 0, 6, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 45 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27466), { 0, 6, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 45 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27452), { 0, 0, height + 24 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27460), { 0, 14, height + 24 },
                        { 32, 2, 63 }, { 0, 29, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27457), { 0, 6, height + 24 },
                        { 32, 2, 3 }, { 0, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27465), { 0, 6, height + 24 },
                        { 2, 2, 3 }, { 29, 20, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27453), { 16, 0, height - 3 },
                        { 5, 2, 119 }, { 16, 29, height + 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27461), { 12, 0, height - 3 },
                        { 32, 2, 119 }, { 0, -6, height + 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27456), { 10, 16, height - 3 },
                        { 4, 2, 119 }, { 10, 0, height + 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27464), { 16, 16, height - 3 },
                        { 2, 2, 119 }, { 29, 29, height + 2 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27454), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 34 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27462), { 0, 0, height + 29 },
                        { 2, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27455), { 0, 16, height + 29 },
                        { 32, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27463), { 0, 16, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 34 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 32, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

static void FlyingRCTrackHalfLoopUninvertedDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackHalfLoopInvertedUp(session, ride, 3 - trackSequence, direction, height, trackElement);
}

static void FlyingRCTrackLeftFlyingLargeHalfLoopInvertedUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27639), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27646), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27653), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27660), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27640), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27647), { 0, 0, height - 4 },
                        { 32, 20, 9 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27654), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27661), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27641), { 0, 0, height - 4 },
                        { 24, 16, 3 }, { 8, 0, height + 40 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27648), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27655), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27662), { 0, 0, height - 4 },
                        { 2, 16, 3 }, { 30, 16, height + 70 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27642), { 0, 0, height - 4 },
                        { 8, 16, 3 }, { 24, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27649), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27656), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27663), { 0, 0, height - 4 },
                        { 2, 16, 3 }, { 30, 16, height + 100 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27643), { 0, 0, height - 4 },
                        { 2, 2, 3 }, { 30, 30, height + 80 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27650), { 0, 0, height - 4 },
                        { 16, 16, 0 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27657), { 0, 0, height - 4 },
                        { 16, 16, 0 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27664), { 0, 0, height - 4 },
                        { 2, 16, 3 }, { 30, 0, height + 90 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27644), { 0, 0, height - 4 },
                        { 2, 16, 3 }, { 30, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27651), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27658), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27665), { 0, 0, height - 4 },
                        { 2, 16, 3 }, { 30, 0, height + 140 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27645), { 0, 0, height - 4 },
                        { 32, 24, 0 }, { 0, 16, height + 32 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 24, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27652), { 0, 0, height - 4 },
                        { 32, 24, 0 }, { 0, 16, height + 32 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 24, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27659), { 0, 0, height - 4 },
                        { 32, 24, 0 }, { 0, 0, height + 32 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 24, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27666), { 0, 0, height - 4 },
                        { 32, 24, 0 }, { 0, 0, height + 32 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 24, session.TrackColours[SCHEME_SUPPORTS]);
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

static void FlyingRCTrackRightFlyingLargeHalfLoopInvertedUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27688), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27681), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27674), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27667), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27689), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27682), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27675), { 0, 0, height - 4 },
                        { 32, 20, 19 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27668), { 0, 0, height - 4 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27690), { 0, 0, height - 4 },
                        { 32, 16, 3 }, { 0, 16, height + 70 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27683), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27676), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27669), { 0, 0, height - 4 },
                        { 32, 16, 3 }, { 0, 0, height + 70 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27691), { 0, 0, height - 4 },
                        { 32, 16, 3 }, { 0, 16, height + 100 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27684), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27677), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27670), { 0, 0, height - 4 },
                        { 32, 16, 3 }, { 0, 0, height + 100 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27692), { 0, 0, height - 4 },
                        { 16, 16, 3 }, { 16, 7, height + 90 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27685), { 0, 0, height - 4 },
                        { 16, 16, 0 }, { 0, -7, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27678), { 0, 0, height - 4 },
                        { 16, 16, 0 }, { -7, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27671), { 0, 0, height - 4 },
                        { 2, 16, 3 }, { 30, 23, height + 110 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27693), { 0, 0, height - 4 },
                        { 2, 16, 3 }, { 30, 7, height + 140 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27686), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27679), { 0, 0, height - 4 },
                        { 32, 16, 0 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27672), { 0, 0, height - 4 },
                        { 2, 16, 3 }, { 30, 16, height + 140 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27694), { 0, 0, height - 4 },
                        { 32, 24, 0 }, { 0, 0, height + 32 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 24, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27687), { 0, 0, height - 4 },
                        { 32, 24, 0 }, { 0, 0, height + 32 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 24, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27680), { 0, 0, height - 4 },
                        { 32, 24, 0 }, { 0, 16, height + 32 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 24, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27673), { 0, 0, height - 4 },
                        { 32, 24, 0 }, { 0, 16, height + 32 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 24, session.TrackColours[SCHEME_SUPPORTS]);
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

static void FlyingRCTrackLeftFlyingLargeHalfLoopUninvertedDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackLeftFlyingLargeHalfLoopInvertedUp(session, ride, 6 - trackSequence, direction, height, trackElement);
}

static void FlyingRCTrackRightFlyingLargeHalfLoopUninvertedDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    FlyingRCTrackRightFlyingLargeHalfLoopInvertedUp(session, ride, 6 - trackSequence, direction, height, trackElement);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionFlyingRC(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return FlyingRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return FlyingRCTrackStation;
        case TrackElemType::Up25:
            return FlyingRCTrack25DegUp;
        case TrackElemType::Up60:
            return FlyingRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return FlyingRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return FlyingRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return FlyingRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return FlyingRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return FlyingRCTrack25DegDown;
        case TrackElemType::Down60:
            return FlyingRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return FlyingRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return FlyingRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return FlyingRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return FlyingRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return FlyingRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return FlyingRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return FlyingRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return FlyingRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return FlyingRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return FlyingRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return FlyingRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return FlyingRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return FlyingRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return FlyingRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return FlyingRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return FlyingRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return FlyingRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return FlyingRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return FlyingRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return FlyingRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return FlyingRCTrackLeftBank;
        case TrackElemType::RightBank:
            return FlyingRCTrackRightBank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return FlyingRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return FlyingRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return FlyingRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return FlyingRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::SBendLeft:
            return FlyingRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return FlyingRCTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return FlyingRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return FlyingRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return FlyingRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return FlyingRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return FlyingRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return FlyingRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return FlyingRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return FlyingRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return FlyingRCTrackLeftHalfBankedHelixUpSmall;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return FlyingRCTrackRightHalfBankedHelixUpSmall;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return FlyingRCTrackLeftHalfBankedHelixDownSmall;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return FlyingRCTrackRightHalfBankedHelixDownSmall;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return FlyingRCTrackLeftHalfBankedHelixUpLarge;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return FlyingRCTrackRightHalfBankedHelixUpLarge;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return FlyingRCTrackLeftHalfBankedHelixDownLarge;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return FlyingRCTrackRightHalfBankedHelixDownLarge;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return FlyingRCTrackLeftQuarterTurn160DegUp;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return FlyingRCTrackRightQuarterTurn160DegUp;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return FlyingRCTrackLeftQuarterTurn160DegDown;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return FlyingRCTrackRightQuarterTurn160DegDown;
        case TrackElemType::Brakes:
            return FlyingRCTrackBrakes;
        case TrackElemType::LeftQuarterBankedHelixLargeUp:
            return FlyingRCTrackLeftQuarterBankedHelixLargeUp;
        case TrackElemType::RightQuarterBankedHelixLargeUp:
            return FlyingRCTrackRightQuarterBankedHelixLargeUp;
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
            return FlyingRCTrackLeftQuarterBankedHelixLargeDown;
        case TrackElemType::RightQuarterBankedHelixLargeDown:
            return FlyingRCTrackRightQuarterBankedHelixLargeDown;
        case TrackElemType::Up25LeftBanked:
            return FlyingRCTrack25DegUpLeftBanked;
        case TrackElemType::Up25RightBanked:
            return FlyingRCTrack25DegUpRightBanked;
        case TrackElemType::OnRidePhoto:
            return FlyingRCTrackOnRidePhoto;
        case TrackElemType::Down25LeftBanked:
            return FlyingRCTrack25DegDownLeftBanked;
        case TrackElemType::Down25RightBanked:
            return FlyingRCTrack25DegDownRightBanked;
        case TrackElemType::LeftEighthToDiag:
            return FlyingRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return FlyingRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return FlyingRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return FlyingRCTrackRightEighthToOrthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return FlyingRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return FlyingRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return FlyingRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return FlyingRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlat:
            return FlyingRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return FlyingRCTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return FlyingRCTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return FlyingRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return FlyingRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return FlyingRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return FlyingRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return FlyingRCTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return FlyingRCTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return FlyingRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return FlyingRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return FlyingRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return FlyingRCTrackDiag25DegDownToFlat;
        case TrackElemType::DiagFlatToLeftBank:
            return FlyingRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return FlyingRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return FlyingRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return FlyingRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return FlyingRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return FlyingRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return FlyingRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return FlyingRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return FlyingRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return FlyingRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return FlyingRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return FlyingRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return FlyingRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return FlyingRCTrackDiagRightBank;
        case TrackElemType::LeftFlyerTwistUp:
            return FlyingRCTrackLeftFlyerTwistUp;
        case TrackElemType::RightFlyerTwistUp:
            return FlyingRCTrackRightFlyerTwistUp;
        case TrackElemType::LeftFlyerTwistDown:
            return FlyingRCTrackLeftFlyerTwistDown;
        case TrackElemType::RightFlyerTwistDown:
            return FlyingRCTrackRightFlyerTwistDown;
        case TrackElemType::FlyerHalfLoopUninvertedUp:
            return FlyingRCTrackFlyerHalfLoopUp;
        case TrackElemType::FlyerHalfLoopInvertedDown:
            return FlyingRCTrackFlyerHalfLoopDown;
        case TrackElemType::BlockBrakes:
            return FlyingRCTrackBlockBrakes;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
            return FlyingRCTrackLeftBankedQuarterTurn325DegUp;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
            return FlyingRCTrackRightBankedQuarterTurn325DegUp;
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return FlyingRCTrackLeftBankedQuarterTurn325DegDown;
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return FlyingRCTrackRightBankedQuarterTurn325DegDown;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
            return FlyingRCTrackLeftBankedQuarterTurn525DegUp;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
            return FlyingRCTrackRightBankedQuarterTurn525DegUp;
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return FlyingRCTrackLeftBankedQuarterTurn525DegDown;
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return FlyingRCTrackRightBankedQuarterTurn525DegDown;
        case TrackElemType::Up25ToLeftBankedUp25:
            return FlyingRCTrack25DegUpToLeftBanked25DegUp;
        case TrackElemType::Up25ToRightBankedUp25:
            return FlyingRCTrack25DegUpToRightBanked25DegUp;
        case TrackElemType::LeftBankedUp25ToUp25:
            return FlyingRCTrackLeftBanked25DegUpTo25DegUp;
        case TrackElemType::RightBankedUp25ToUp25:
            return FlyingRCTrackRightBanked25DegUpTo25DegUp;
        case TrackElemType::Down25ToLeftBankedDown25:
            return FlyingRCTrack25DegDownToLeftBanked25DegDown;
        case TrackElemType::Down25ToRightBankedDown25:
            return FlyingRCTrack25DegDownToRightBanked25DegDown;
        case TrackElemType::LeftBankedDown25ToDown25:
            return FlyingRCTrackLeftBanked25DegDownTo25DegDown;
        case TrackElemType::RightBankedDown25ToDown25:
            return FlyingRCTrackRightBanked25DegDownTo25DegDown;
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
            return FlyingRCTrackLeftBankedFlatToLeftBanked25DegUp;
        case TrackElemType::RightBankedFlatToRightBankedUp25:
            return FlyingRCTrackRightBankedFlatToRightBanked25DegUp;
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
            return FlyingRCTrackLeftBanked25DegUpToLeftBankedFlat;
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
            return FlyingRCTrackRightBanked25DegUpToRightBankedFlat;
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
            return FlyingRCTrackLeftBankedFlatToLeftBanked25DegDown;
        case TrackElemType::RightBankedFlatToRightBankedDown25:
            return FlyingRCTrackRightBankedFlatToRightBanked25DegDown;
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
            return FlyingRCTrackLeftBanked25DegDownToLeftBankedFlat;
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
            return FlyingRCTrackRightBanked25DegDownToRightBankedFlat;
        case TrackElemType::FlatToLeftBankedUp25:
            return FlyingRCTrackFlatToLeftBanked25DegUp;
        case TrackElemType::FlatToRightBankedUp25:
            return FlyingRCTrackFlatToRightBanked25DegUp;
        case TrackElemType::LeftBankedUp25ToFlat:
            return FlyingRCTrackLeftBanked25DegUpToFlat;
        case TrackElemType::RightBankedUp25ToFlat:
            return FlyingRCTrackRightBanked25DegUpToFlat;
        case TrackElemType::FlatToLeftBankedDown25:
            return FlyingRCTrackFlatToLeftBanked25DegDown;
        case TrackElemType::FlatToRightBankedDown25:
            return FlyingRCTrackFlatToRightBanked25DegDown;
        case TrackElemType::LeftBankedDown25ToFlat:
            return FlyingRCTrackLeftBanked25DegDownToFlat;
        case TrackElemType::RightBankedDown25ToFlat:
            return FlyingRCTrackRightBanked25DegDownToFlat;
        // OpenRCT2-specific paint code
        case TrackElemType::Booster:
            return FlyingRCTrackBooster;
        case TrackElemType::Up60ToUp90:
            return FlyingRCTrack60DegUpTo90DegUp;
        case TrackElemType::Up90:
            return FlyingRCTrack90DegUp;
        case TrackElemType::Up90ToUp60:
            return FlyingRCTrack90DegUpTo60DegUp;
        case TrackElemType::Down60ToDown90:
            return FlyingRCTrack60DegDownTo90DegDown;
        case TrackElemType::Down90:
            return FlyingRCTrack90DegDown;
        case TrackElemType::Down90ToDown60:
            return FlyingRCTrack90DegDownTo60DegDown;
        case TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop:
            return FlyingRCTrackInvertedFlatTo90DegQuarterLoopDown;
        case TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop:
            return FlyingRCTrack90DegToInvertedFlatQuarterLoopUp;
        case TrackElemType::FlatToUp60LongBase:
            return FlyingRCTrackFlatTo60DegUpLongBase;
        case TrackElemType::Up60ToFlatLongBase:
            return FlyingRCTrack60DegUpToFlatLongBase;
        case TrackElemType::FlatToDown60LongBase:
            return FlyingRCTrackFlatTo60DegDownLongBase;
        case TrackElemType::Down60ToFlatLongBase:
            return FlyingRCTrack60DegDownToFlatLongBase;
            // OpenRCT2-specific track elements
        case TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp:
            return BolligerMabillardTrackLeftLargeHalfLoopUp<METAL_SUPPORTS_TUBES_INVERTED>;
        case TrackElemType::RightFlyerLargeHalfLoopUninvertedUp:
            return BolligerMabillardTrackRightLargeHalfLoopUp<METAL_SUPPORTS_TUBES_INVERTED>;
        case TrackElemType::RightFlyerLargeHalfLoopInvertedDown:
            return BolligerMabillardTrackRightLargeHalfLoopDown<METAL_SUPPORTS_TUBES_INVERTED>;
        case TrackElemType::LeftFlyerLargeHalfLoopInvertedDown:
            return BolligerMabillardTrackLeftLargeHalfLoopDown<METAL_SUPPORTS_TUBES_INVERTED>;
        case TrackElemType::FlyerHalfLoopInvertedUp:
            return FlyingRCTrackHalfLoopInvertedUp;
        case TrackElemType::FlyerHalfLoopUninvertedDown:
            return FlyingRCTrackHalfLoopUninvertedDown;
        case TrackElemType::LeftFlyerLargeHalfLoopInvertedUp:
            return FlyingRCTrackLeftFlyingLargeHalfLoopInvertedUp;
        case TrackElemType::RightFlyerLargeHalfLoopInvertedUp:
            return FlyingRCTrackRightFlyingLargeHalfLoopInvertedUp;
        case TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown:
            return FlyingRCTrackLeftFlyingLargeHalfLoopUninvertedDown;
        case TrackElemType::RightFlyerLargeHalfLoopUninvertedDown:
            return FlyingRCTrackRightFlyingLargeHalfLoopUninvertedDown;
    }
    return GetTrackPaintFunctionBolligerMabillard<METAL_SUPPORTS_TUBES>(trackType);
}
