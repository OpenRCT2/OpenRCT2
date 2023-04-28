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

/** rct2: 0x0082491C */
static void LayDownRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::Flat);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26557), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 1 } });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26558), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 1 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26555), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 1 } });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26556), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 1 } });
                    break;
            }
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824B8C, 0x00824B9C, 0x00824BAC */
static void LayDownRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { SPR_STATION_BASE_C_SW_NE, 26557, SPR_STATION_INVERTED_BAR_A_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 26558, SPR_STATION_INVERTED_BAR_A_NW_SE },
            { SPR_STATION_BASE_C_SW_NE, 26557, SPR_STATION_INVERTED_BAR_A_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 26558, SPR_STATION_INVERTED_BAR_A_NW_SE },
        };

        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][0]), { 0, 0, height },
            { { 0, 2, height }, { 32, 28, 1 } });
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]), { 0, 0, height + 24 },
            { { 0, 6, height + 24 }, { 32, 20, 3 } });
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours[SCHEME_SUPPORTS].WithIndex(imageIds[direction][2]), { 0, 6, height + 24 },
            { { 0, 6, height + 24 }, { 32, 20, 1 } });
        TrackPaintUtilDrawStationMetalSupports2(
            session, direction, height, session.TrackColours[SCHEME_SUPPORTS], MetalSupportType::TubesInverted);
        TrackPaintUtilDrawStationInverted(session, ride, direction, height, trackElement, STATION_VARIANT_1);
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
    else
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(trackElement.GetTrackType());
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
}

/** rct2: 0x0082492C */
static void LayDownRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::Up25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26621), { 0, 0, height + 24 },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26622), { 0, 0, height + 24 },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26623), { 0, 0, height + 24 },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26624), { 0, 0, height + 24 },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26569), { 0, 0, height + 24 },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26570), { 0, 0, height + 24 },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26571), { 0, 0, height + 24 },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26572), { 0, 0, height + 24 },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
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
                        session, MetalSupportType::TubesInverted, 6, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 8, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 7, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 5, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
    }
}

/** rct2: 0x0082493C */
static void LayDownRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::Up60);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26585), { 0, 0, height + 24 },
                    { { 0, 6, height + 88 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26586), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 81 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26587), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 81 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26588), { 0, 0, height + 24 },
                    { { 0, 6, height + 88 }, { 32, 20, 3 } });
                break;
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
}

/** rct2: 0x0082494C */
static void LayDownRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::FlatToUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26613), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26614), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26615), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26616), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26561), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26562), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26563), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26564), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
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
                        session, MetalSupportType::TubesInverted, 6, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 8, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 7, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 5, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }
}

/** rct2: 0x0082495C */
static void LayDownRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::Up25ToUp60);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26573), { 0, 0, height + 24 },
                    { { 0, 6, height + 56 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26577), { 0, 0, height + 24 },
                    { { 0, 10, height + 6 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26574), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 49 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26578), { 0, 0, height + 24 },
                    { { 0, 10, height + 6 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26575), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 49 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26576), { 0, 0, height + 24 },
                    { { 0, 6, height + 56 }, { 32, 20, 3 } });
                break;
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
}

/** rct2: 0x0082496C */
static void LayDownRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::Up60ToUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26579), { 0, 0, height + 24 },
                    { { 0, 6, height + 56 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26583), { 0, 0, height + 24 },
                    { { 0, 10, height + 6 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26580), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 49 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26584), { 0, 0, height + 24 },
                    { { 0, 10, height + 6 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26581), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 49 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26582), { 0, 0, height + 24 },
                    { { 0, 6, height + 56 }, { 32, 20, 3 } });
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
                        session, MetalSupportType::TubesInverted, 6, 0, height + 65, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 8, 0, height + 65, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 7, 0, height + 65, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 5, 0, height + 65, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }
}

/** rct2: 0x0082497C */
static void LayDownRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::Up25ToFlat);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26617), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26618), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26619), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26620), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26565), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26566), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26567), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26568), { 0, 0, height + 24 },
                        { { 0, 6, height + 32 }, { 32, 20, 3 } });
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
                        session, MetalSupportType::TubesInverted, 6, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 8, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 7, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 5, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
    }
}

/** rct2: 0x0082498C */
static void LayDownRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0082499C */
static void LayDownRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008249AC */
static void LayDownRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008249BC */
static void LayDownRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008249CC */
static void LayDownRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008249DC */
static void LayDownRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008249EC */
static void LayDownRCTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::LeftQuarterTurn5Tiles);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26694), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26699), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26704), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26689), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26693), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26698), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26703), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26688), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26692), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26697), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26702), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26687), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26691), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26696), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26701), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26686), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26690), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26695), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26700), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26685), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x008249FC */
static void LayDownRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    LayDownRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824A0C */
static void LayDownRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::FlatToLeftBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26657), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26658), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26659), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26660), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824A1C */
static void LayDownRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::FlatToRightBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26661), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26662), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26663), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26664), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824A2C */
static void LayDownRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::LeftBankToFlat);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26663), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26664), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26661), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26662), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824A3C */
static void LayDownRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::RightBankToFlat);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26659), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26660), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26657), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26658), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824A4C */
static void LayDownRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::BankedLeftQuarterTurn5Tiles);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26738), { 0, 0, height + 24 },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26743), { 0, 0, height + 24 },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26748), { 0, 0, height + 24 },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26733), { 0, 0, height + 24 },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26737), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26742), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26747), { 0, 0, height + 24 },
                            { { 0, 16, height + 24 }, { 32, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26732), { 0, 0, height + 24 },
                            { { 0, 16, height + 24 }, { 32, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26736), { 0, 0, height + 24 },
                            { { 0, 16, height + 24 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26741), { 0, 0, height + 24 },
                            { { 16, 16, height + 24 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26746), { 0, 0, height + 24 },
                            { { 16, 0, height + 24 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26731), { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26735), { 0, 0, height + 24 },
                            { { 16, 0, height + 24 }, { 16, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26740), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26745), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26730), { 0, 0, height + 24 },
                            { { 16, 0, height + 24 }, { 16, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26734), { 0, 0, height + 24 },
                            { { 6, 0, height + 24 }, { 20, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26739), { 0, 0, height + 24 },
                            { { 6, 0, height + 24 }, { 20, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26744), { 0, 0, height + 24 },
                            { { 6, 0, height + 24 }, { 20, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26729), { 0, 0, height + 24 },
                            { { 6, 0, height + 24 }, { 20, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824A5C */
static void LayDownRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    LayDownRCTrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824A6C */
static void LayDownRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::LeftBankToUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26665), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26666), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26667), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26668), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
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
                        session, MetalSupportType::TubesInverted, 6, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 8, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 7, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 5, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }
}

/** rct2: 0x00824A7C */
static void LayDownRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::RightBankToUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26669), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26670), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26671), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26672), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
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
                        session, MetalSupportType::TubesInverted, 6, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 8, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 7, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 5, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }
}

/** rct2: 0x00824A8C */
static void LayDownRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::Up25ToLeftBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26673), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26674), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26675), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26676), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
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
                        session, MetalSupportType::TubesInverted, 6, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 8, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 7, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 5, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
    }
}

/** rct2: 0x00824A9C */
static void LayDownRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::Up25ToRightBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26677), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26678), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26679), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26680), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
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
                        session, MetalSupportType::TubesInverted, 6, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 8, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 7, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 5, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
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
        PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
    }
}

/** rct2: 0x00824AAC */
static void LayDownRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824ABC */
static void LayDownRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824ACC */
static void LayDownRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824ADC */
static void LayDownRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824AEC */
static void LayDownRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::LeftBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26681), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26682), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26683), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26684), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824AFC */
static void LayDownRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824B0C */
static void LayDownRCTrackLeftQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::LeftQuarterTurn5TilesUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26949), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26954), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26959), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26964), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26950), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26955), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26960), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26965), { 0, 16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26951), { 0, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26956), { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26961), { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26966), { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26952), { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26957), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26962), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26967), { 16, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26953), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26958), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26963), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26968), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824B1C */
static void LayDownRCTrackRightQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::RightQuarterTurn5TilesUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26929), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26934), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26939), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26944), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26930), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26935), { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26940), { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26945), { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26931), { 0, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26936), { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26941), { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26946), { 0, 16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26932), { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26937), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26942), { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26947), { 16, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26933), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26938), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26943), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26948), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824B2C */
static void LayDownRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    LayDownRCTrackRightQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x00824B3C */
static void LayDownRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    LayDownRCTrackLeftQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824B4C */
static void LayDownRCTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::SBendLeft);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26641), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26645), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26644), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26648), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26642), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26646), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26643), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26647), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
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
                            session, MetalSupportType::TubesInverted, 5, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 6, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26643), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26647), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26642), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26646), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
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
                            session, MetalSupportType::TubesInverted, 5, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 6, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26644), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26648), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26641), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26645), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824B5C */
static void LayDownRCTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::SBendRight);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26649), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26653), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26652), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26656), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26650), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26654), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26651), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26655), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
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
                            session, MetalSupportType::TubesInverted, 8, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 7, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26651), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26655), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26650), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26654), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
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
                            session, MetalSupportType::TubesInverted, 8, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 7, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26652), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26656), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26649), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26653), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824BBC */
static void LayDownRCTrackLeftQuarterTurn3(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16381), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16384), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16387), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16378), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16380), { 0, 0, height },
                            { { 16, 0, height }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16383), { 0, 0, height },
                            { { 0, 0, height }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16386), { 0, 0, height },
                            { { 0, 16, height }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16377), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16379), { 0, 0, height },
                            { { 6, 0, height }, { 20, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16382), { 0, 0, height },
                            { { 6, 0, height }, { 20, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16385), { 0, 0, height },
                            { { 6, 0, height }, { 20, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16376), { 0, 0, height },
                            { { 6, 0, height }, { 20, 32, 3 } });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26710), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26713), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26716), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26707), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26709), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26712), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26715), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26706), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26708), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26711), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26714), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26705), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824BCC */
static void LayDownRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    LayDownRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824BDC */
static void LayDownRCTrackLeftQuarterTurn3Bank(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16393), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16400), { 0, 0, height },
                            { { 0, 27, height }, { 32, 1, 26 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16396), { 0, 0, height },
                            { { 0, 27, height }, { 32, 1, 26 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16399), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16390), { 0, 0, height },
                            { { 0, 6, height }, { 32, 20, 3 } });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16392), { 0, 0, height },
                            { { 16, 0, height }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16395), { 0, 0, height },
                            { { 0, 0, height + 27 }, { 16, 16, 1 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16398), { 0, 0, height },
                            { { 0, 16, height }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16389), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16391), { 0, 0, height },
                            { { 6, 0, height }, { 20, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16394), { 0, 0, height },
                            { { 27, 0, height }, { 1, 32, 26 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16397), { 0, 0, height },
                            { { 6, 0, height }, { 20, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16401), { 0, 0, height },
                            { { 27, 0, height }, { 1, 32, 26 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16388), { 0, 0, height },
                            { { 6, 0, height }, { 20, 32, 3 } });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26722), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26725), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26728), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26719), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26721), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26724), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26727), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26718), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26720), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26723), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26726), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26717), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824BEC */
static void LayDownRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    LayDownRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824BFC */
static void LayDownRCTrackLeftQuarterTurn325DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16541), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16543), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16545), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16539), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16540), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16542), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16544), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16538), { 6, 0, height },
                            { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26924), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26926), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26928), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26922), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26923), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26925), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26927), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26921), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824C0C */
static void LayDownRCTrackRightQuarterTurn325DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16530), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16532), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16534), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16536), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16531), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16533), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16535), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16537), { 6, 0, height },
                            { 20, 32, 3 });
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
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
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26913), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26915), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26917), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26919), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26914), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26916), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26918), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26920), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824C1C */
static void LayDownRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    LayDownRCTrackRightQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x00824C2C */
static void LayDownRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    LayDownRCTrackLeftQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824D3C */
static void LayDownRCTrackLeftQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::LeftQuarterTurn1TileUp60);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26910), { 0, 0, height },
                    { { 2, 2, height }, { 28, 28, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26906), { 0, 0, height },
                    { { 2, 2, height + 99 }, { 28, 28, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26911), { 0, 0, height },
                    { { 2, 2, height }, { 28, 28, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26907), { 0, 0, height },
                    { { 2, 2, height + 99 }, { 28, 28, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26912), { 0, 0, height },
                    { { 2, 2, height }, { 28, 28, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26908), { 0, 0, height },
                    { { 2, 2, height + 99 }, { 28, 28, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26909), { 0, 0, height },
                    { { 2, 2, height }, { 28, 28, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26905), { 0, 0, height },
                    { { 2, 2, height + 99 }, { 28, 28, 1 } });
                break;
        }

        TrackPaintUtilLeftQuarterTurn1TileTunnel(session, direction, height, -8, TUNNEL_1, +56, TUNNEL_2);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
    }
}

/** rct2: 0x00824D1C */
static void LayDownRCTrackRightQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::RightQuarterTurn1TileUp60);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26901), { 0, 0, height },
                    { { 2, 2, height }, { 28, 28, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26897), { 0, 0, height },
                    { { 2, 2, height + 99 }, { 28, 28, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26902), { 0, 0, height },
                    { { 2, 2, height }, { 28, 28, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26898), { 0, 0, height },
                    { { 2, 2, height + 99 }, { 28, 28, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26903), { 0, 0, height },
                    { { 2, 2, height }, { 28, 28, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26899), { 0, 0, height },
                    { { 2, 2, height + 99 }, { 28, 28, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26904), { 0, 0, height },
                    { { 2, 2, height }, { 28, 28, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26900), { 0, 0, height },
                    { { 2, 2, height + 99 }, { 28, 28, 1 } });
                break;
        }
    }

    TrackPaintUtilRightQuarterTurn1TileTunnel(session, direction, height, -8, TUNNEL_1, +56, TUNNEL_2);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x00824D2C */
static void LayDownRCTrackLeftQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrackRightQuarterTurn160DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x00824D4C */
static void LayDownRCTrackRightQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrackLeftQuarterTurn160DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824D5C */
static void LayDownRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::Brakes);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26559), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26560), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824D6C */
static void LayDownRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::OnRidePhoto);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26555), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26556), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26555), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26556), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
        }
        TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
    }
}

/** rct2: 0x00824D8C */
static void LayDownRCTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::LeftEighthToDiag);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26765), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26769), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26773), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26777), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26766), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26770), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 34, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26774), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26778), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26767), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26771), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26775), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26779), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26768), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26772), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 18, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26776), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26780), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
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
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824D9C */
static void LayDownRCTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::RightEighthToDiag);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26749), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26753), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26757), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26761), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26750), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26754), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26758), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 34, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26762), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26751), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26755), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26759), { 0, 0, height + 24 },
                            { { 4, 4, height + 22 }, { 28, 28, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26763), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26752), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26756), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26760), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 18, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26764), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
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
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824DAC */
static void LayDownRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    LayDownRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824DBC */
static void LayDownRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    LayDownRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x00824DCC */
static void LayDownRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::LeftEighthBankToDiag);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26853), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26857), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26861), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26865), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26854), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26858), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 34, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26862), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26866), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26855), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26859), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26863), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26867), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26856), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26860), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 18, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26864), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26868), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
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
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824DDC */
static void LayDownRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::RightEighthBankToDiag);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26837), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26841), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26845), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26849), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26838), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26842), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26846), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 34, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26850), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26839), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26843), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26847), { 0, 0, height + 24 },
                            { { 4, 4, height + 22 }, { 28, 28, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26851), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26840), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26844), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26848), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 18, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26852), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
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
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824DEC */
static void LayDownRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    LayDownRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824DFC */
static void LayDownRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    LayDownRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x00824D7C */
static void LayDownRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagFlat);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26812),
                                { -16, -16, height + 24 }, { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26784),
                                { -16, -16, height + 24 }, { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26809),
                                { -16, -16, height + 24 }, { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26781),
                                { -16, -16, height + 24 }, { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26811),
                                { -16, -16, height + 24 }, { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26783),
                                { -16, -16, height + 24 }, { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26810),
                                { -16, -16, height + 24 }, { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26782),
                                { -16, -16, height + 24 }, { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E2C */
static void LayDownRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26824),
                                { -16, -16, height + 24 }, { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26796),
                                { -16, -16, height + 24 }, { { -16, -16, height + 40 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26821),
                                { -16, -16, height + 24 }, { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26793),
                                { -16, -16, height + 24 }, { { -16, -16, height + 40 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26823),
                                { -16, -16, height + 24 }, { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26795),
                                { -16, -16, height + 24 }, { { -16, -16, height + 40 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26822),
                                { -16, -16, height + 24 }, { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26794),
                                { -16, -16, height + 24 }, { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E8C */
static void LayDownRCTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagUp60);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26808), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26805), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26807), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26806), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 32, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 36, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 32, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 36, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E0C */
static void LayDownRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagFlatToUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26816),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26788),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26813),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26785),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26815),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26787),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26814),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26786),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E6C */
static void LayDownRCTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagUp25ToUp60);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26800), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26797), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26799), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26798), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 16, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 16, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 16, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 16, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E7C */
static void LayDownRCTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagUp60ToUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26804), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26801), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26803), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26802), { -16, -16, height + 24 },
                            { { 0, 0, height + 56 }, { 16, 16, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 21, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 21, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 21, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 21, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E1C */
static void LayDownRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagUp25ToFlat);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26820),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26792),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26817),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26789),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26819),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26791),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26818),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26790),
                                { -16, -16, height + 24 }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E5C */
static void LayDownRCTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagDown25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26794), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26795), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26793), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26796), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824EBC */
static void LayDownRCTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagDown60);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26806), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26807), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26805), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26808), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 24, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 28, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 24, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 28, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E3C */
static void LayDownRCTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagFlatToDown25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26790), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26791), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26789), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26792), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                break;
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x00824E9C */
static void LayDownRCTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagDown25ToDown60);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26802), { -16, -16, height + 24 },
                            { { 0, 0, height + 56 }, { 16, 16, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26803), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26801), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26804), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 17, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 17, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 17, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 17, height + 33,
                            session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824EAC */
static void LayDownRCTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagDown60ToDown25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26798), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26799), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26797), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26800), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 8, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 8, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 8, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 8, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E4C */
static void LayDownRCTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagDown25ToFlat);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26786), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26787), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26785), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26788), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824EEC */
static void LayDownRCTrackDiagFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagFlatToLeftBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26876), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26873), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26875), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26874), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824EFC */
static void LayDownRCTrackDiagFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagFlatToRightBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26880), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26877), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26879), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26878), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F0C */
static void LayDownRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagLeftBankToFlat);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26878), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26879), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26877), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26880), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F1C */
static void LayDownRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagRightBankToFlat);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26874), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26875), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26873), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26876), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F4C */
static void LayDownRCTrackDiagLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagLeftBankToUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26892), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26889), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26891), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26890), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F5C */
static void LayDownRCTrackDiagRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagRightBankToUp25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26896), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26893), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26895), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26894), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F2C */
static void LayDownRCTrackDiag25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagUp25ToLeftBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26884), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26881), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26883), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26882), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F3C */
static void LayDownRCTrackDiag25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagUp25ToRightBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26888), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26885), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26887), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26886), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F6C */
static void LayDownRCTrackDiagLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagLeftBankToDown25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26886), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26887), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26885), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26888), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                break;
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x00824F7C */
static void LayDownRCTrackDiagRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagRightBankToDown25);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26882), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26883), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26881), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26884), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                break;
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x00824F8C */
static void LayDownRCTrackDiag25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagDown25ToLeftBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26894), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26895), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26893), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26896), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F9C */
static void LayDownRCTrackDiag25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagDown25ToRightBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26890), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26891), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26889), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26892), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824ECC */
static void LayDownRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagLeftBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26872), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26869), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26871), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26870), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824EDC */
static void LayDownRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::DiagRightBank);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26870), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26871), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26869), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26872), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportType::TubesInverted, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824FAC */
static void LayDownRCTrackLeftFlyerTwistUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26972), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26978), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26971), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26977), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26973), { 0, 6, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26979), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26970), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26976), { 0, 6, height + 24 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26974), { 0, 6, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26980), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26969), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26975), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824FBC */
static void LayDownRCTrackRightFlyerTwistUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26984), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26990), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26983), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26989), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26985), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26991), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26982), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26988), { 0, 6, height + 24 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26986), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26992), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26981), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26987), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824FCC */
static void LayDownRCTrackLeftFlyerTwistDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26969), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26975), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26974), { 0, 6, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26980), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26970), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26976), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26973), { 0, 6, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26979), { 0, 6, height + 24 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26971), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26977), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26972), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26978), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824FDC */
static void LayDownRCTrackRightFlyerTwistDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26981), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26987), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26986), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26992), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26982), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26988), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26985), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26991), { 0, 6, height + 24 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26983), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26989), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26984), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26990), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824C3C */
static void LayDownRCTrackFlyerHalfLoopUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16594), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16602), { 0, 6, height },
                        { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16601), { 0, 6, height },
                        { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16609), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16595), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16603), { 0, 14, height },
                        { { 28, 6, height }, { 3, 20, 63 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16600), { 0, 6, height },
                        { { 28, 6, height }, { 3, 20, 63 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16608), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16596), { 16, 0, height },
                        { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16604), { 12, 0, height },
                        { { 12, 0, height }, { 3, 16, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16599), { 10, 16, height },
                        { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16607), { 16, 16, height },
                        { { 15, 6, height }, { 2, 16, 119 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16597), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16605), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16598), { 0, 16, height + 32 },
                        { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16606), { 0, 16, height + 32 },
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

/** rct2: 0x00824C4C */
static void LayDownRCTrackFlyerHalfLoopDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16597), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16605), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16598), { 0, 16, height + 32 },
                        { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16606), { 0, 16, height + 32 },
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
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16596), { 16, 0, height },
                        { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16604), { 12, 0, height },
                        { { 12, 0, height }, { 3, 16, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16599), { 10, 16, height },
                        { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16607), { 16, 16, height },
                        { { 15, 6, height }, { 2, 16, 119 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16595), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16603), { 0, 14, height },
                        { { 28, 6, height }, { 3, 20, 63 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16600), { 0, 6, height },
                        { { 28, 6, height }, { 3, 20, 63 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16608), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16594), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16602), { 0, 6, height },
                        { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16601), { 0, 6, height },
                        { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16609), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x00824C5C */
static void LayDownRCTrackLeftFlyerCorkscrewUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16610), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16613), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16616), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16619), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
            }

            if (direction == 2)
            {
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
            }
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction != 2)
            {
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
            }

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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16611), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16614), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16617), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16620), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16612), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16615), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16618), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16621), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

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

/** rct2: 0x00824C6C */
static void LayDownRCTrackRightFlyerCorkscrewUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16622), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16625), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16628), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16631), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16623), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16626), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16629), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16632), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16624), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16627), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16630), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16633), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

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

/** rct2: 0x00824C7C */
static void LayDownRCTrackLeftFlyerCorkscrewDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16627), { 0, 0, height },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16630), { 0, 0, height },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16633), { 0, 0, height },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16624), { 0, 0, height },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16626), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16629), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16632), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16623), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16625), { 0, 0, height },
                        { { 6, 0, height + 4 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16628), { 0, 0, height },
                        { { 6, 0, height + 4 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16631), { 0, 0, height },
                        { { 6, 0, height + 4 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16622), { 0, 0, height },
                        { { 6, 0, height + 4 }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x00824C8C */
static void LayDownRCTrackRightFlyerCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrackLeftFlyerCorkscrewUp(session, ride, 2 - trackSequence, (direction + 3) % 4, height, trackElement);
}

/** rct2: 0x00824FEC */
static void LayDownRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        auto function = GetTrackPaintFunctionCorkscrewRC(TrackElemType::BlockBrakes);
        function(session, ride, trackSequence, direction, height, trackElement);
        return;
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26559), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26560), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }
}

static void LayDownRCTrackLeftQuarterBankedHelixLargeUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27118), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27123), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27128), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27113), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27117), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27122), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27127), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27112), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27116), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27121), { 0, 0, height + 24 },
                        { { 16, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27126), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27111), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27115), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27120), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27125), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27110), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27114), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27119), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27124), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27109), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);

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

static void LayDownRCTrackRightQuarterBankedHelixLargeUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27089), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27094), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27099), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27104), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27090), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27095), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27100), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27105), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27091), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27096), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27101), { 0, 0, height + 24 },
                        { { 16, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27106), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27092), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27097), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27102), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27107), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27093), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27098), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27103), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27108), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);

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

static void LayDownRCTrackLeftQuarterBankedHelixLargeDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27098), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27103), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27108), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27093), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);

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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27097), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27102), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27107), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27092), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27096), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27101), { 0, 0, height + 24 },
                        { { 16, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27106), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27091), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27095), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27100), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27105), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27090), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27094), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27099), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27104), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27089), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

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

static void LayDownRCTrackRightQuarterBankedHelixLargeDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27109), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27114), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27119), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27124), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);

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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27110), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27115), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27120), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27125), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27111), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27116), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27121), { 0, 0, height + 24 },
                        { { 16, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27126), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27112), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27117), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27122), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27127), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27113), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27118), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27123), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27128), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportType::TubesInverted, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

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
static void LayDownRCTrackHalfLoopInvertedUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27073), { 0, 6, height + 24 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27081), { 0, 6, height + 24 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27080), { 0, 6, height + 24 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27088), { 0, 6, height + 24 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 6, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 8, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 7, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::TubesInverted, 5, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27074), { 0, 0, height + 24 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27082), { 0, 14, height + 24 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27079), { 0, 6, height + 24 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27087), { 0, 6, height + 24 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27075), { 16, 0, height - 3 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27083), { 12, 0, height - 3 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27078), { 10, 16, height - 3 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27086), { 16, 16, height - 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27076), { 0, 0, height + 29 },
                        { { 0, 6, height + 34 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27084), { 0, 0, height + 29 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27077), { 0, 16, height + 29 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27085), { 0, 16, height + 29 },
                        { { 0, 6, height + 34 }, { 32, 20, 3 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 32, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

static void LayDownRCTrackHalfLoopUninvertedDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    LayDownRCTrackHalfLoopInvertedUp(session, ride, 3 - trackSequence, direction, height, trackElement);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionLayDownRC(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return LayDownRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return LayDownRCTrackStation;
        case TrackElemType::Up25:
            return LayDownRCTrack25DegUp;
        case TrackElemType::Up60:
            return LayDownRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return LayDownRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return LayDownRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return LayDownRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return LayDownRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return LayDownRCTrack25DegDown;
        case TrackElemType::Down60:
            return LayDownRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return LayDownRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return LayDownRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return LayDownRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return LayDownRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return LayDownRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return LayDownRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return LayDownRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return LayDownRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return LayDownRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return LayDownRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return LayDownRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return LayDownRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return LayDownRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return LayDownRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return LayDownRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return LayDownRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return LayDownRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return LayDownRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return LayDownRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return LayDownRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return LayDownRCTrackLeftBank;
        case TrackElemType::RightBank:
            return LayDownRCTrackRightBank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return LayDownRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return LayDownRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return LayDownRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return LayDownRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::SBendLeft:
            return LayDownRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return LayDownRCTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return LayDownRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return LayDownRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return LayDownRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return LayDownRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return LayDownRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return LayDownRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return LayDownRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return LayDownRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return LayDownRCTrackLeftQuarterTurn160DegUp;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return LayDownRCTrackRightQuarterTurn160DegUp;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return LayDownRCTrackLeftQuarterTurn160DegDown;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return LayDownRCTrackRightQuarterTurn160DegDown;
        case TrackElemType::Brakes:
            return LayDownRCTrackBrakes;
        case TrackElemType::OnRidePhoto:
            return LayDownRCTrackOnRidePhoto;
        case TrackElemType::LeftEighthToDiag:
            return LayDownRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return LayDownRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return LayDownRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return LayDownRCTrackRightEighthToOrthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return LayDownRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return LayDownRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return LayDownRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return LayDownRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlat:
            return LayDownRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return LayDownRCTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return LayDownRCTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return LayDownRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return LayDownRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return LayDownRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return LayDownRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return LayDownRCTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return LayDownRCTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return LayDownRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return LayDownRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return LayDownRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return LayDownRCTrackDiag25DegDownToFlat;
        case TrackElemType::DiagFlatToLeftBank:
            return LayDownRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return LayDownRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return LayDownRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return LayDownRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return LayDownRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return LayDownRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return LayDownRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return LayDownRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return LayDownRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return LayDownRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return LayDownRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return LayDownRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return LayDownRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return LayDownRCTrackDiagRightBank;
        case TrackElemType::LeftFlyerTwistUp:
            return LayDownRCTrackLeftFlyerTwistUp;
        case TrackElemType::RightFlyerTwistUp:
            return LayDownRCTrackRightFlyerTwistUp;
        case TrackElemType::LeftFlyerTwistDown:
            return LayDownRCTrackLeftFlyerTwistDown;
        case TrackElemType::RightFlyerTwistDown:
            return LayDownRCTrackRightFlyerTwistDown;
        case TrackElemType::FlyerHalfLoopUninvertedUp:
            return LayDownRCTrackFlyerHalfLoopUp;
        case TrackElemType::FlyerHalfLoopInvertedDown:
            return LayDownRCTrackFlyerHalfLoopDown;
        case TrackElemType::LeftFlyerCorkscrewUp:
            return LayDownRCTrackLeftFlyerCorkscrewUp;
        case TrackElemType::RightFlyerCorkscrewUp:
            return LayDownRCTrackRightFlyerCorkscrewUp;
        case TrackElemType::LeftFlyerCorkscrewDown:
            return LayDownRCTrackLeftFlyerCorkscrewDown;
        case TrackElemType::RightFlyerCorkscrewDown:
            return LayDownRCTrackRightFlyerCorkscrewDown;
        case TrackElemType::BlockBrakes:
            return LayDownRCTrackBlockBrakes;

        case TrackElemType::LeftQuarterBankedHelixLargeUp:
            return LayDownRCTrackLeftQuarterBankedHelixLargeUp;
        case TrackElemType::RightQuarterBankedHelixLargeUp:
            return LayDownRCTrackRightQuarterBankedHelixLargeUp;
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
            return LayDownRCTrackLeftQuarterBankedHelixLargeDown;
        case TrackElemType::RightQuarterBankedHelixLargeDown:
            return LayDownRCTrackRightQuarterBankedHelixLargeDown;

        case TrackElemType::FlyerHalfLoopInvertedUp:
            return LayDownRCTrackHalfLoopInvertedUp;
        case TrackElemType::FlyerHalfLoopUninvertedDown:
            return LayDownRCTrackHalfLoopUninvertedDown;
    }
    return GetTrackPaintFunctionCorkscrewRC(trackType);
}
