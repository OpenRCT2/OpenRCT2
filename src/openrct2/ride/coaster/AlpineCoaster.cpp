#pragma region Copyright(c) 2014 - 2022 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"
namespace OpenRCT2
{
    namespace AlpineRC
    {
        static void TrackFlat(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_FLAT + 0)), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_FLAT + 1)), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_FLAT + 2)), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_FLAT + 3)), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_FLAT + 0)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_FLAT + 1)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
        }

        static void TrackStation(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            static constexpr const uint32_t imageIds[4][3] = {
                { (SPR_G2_ALPINE_TRACK_FLAT + 0), (SPR_G2_ALPINE_TRACK_FLAT + 0), SPR_STATION_BASE_B_SW_NE },
                { (SPR_G2_ALPINE_TRACK_FLAT + 1), (SPR_G2_ALPINE_TRACK_FLAT + 1), SPR_STATION_BASE_B_NW_SE },
                { (SPR_G2_ALPINE_TRACK_FLAT + 0), (SPR_G2_ALPINE_TRACK_FLAT + 0), SPR_STATION_BASE_B_SW_NE },
                { (SPR_G2_ALPINE_TRACK_FLAT + 1), (SPR_G2_ALPINE_TRACK_FLAT + 1), SPR_STATION_BASE_B_NW_SE },
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
                session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][2]), { 0, 0, height - 2 },
                { 32, 28, 2 }, { 0, 2, height });
            TrackPaintUtilDrawStationMetalSupports(session, direction, height, session.TrackColours[SCHEME_SUPPORTS]);
            TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, 4, 7);
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
        }

        static void Track25DegUp(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 8)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 9)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 10)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 11)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 8, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 8)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 9)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 10)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 11)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 8, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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

        static void TrackFlatTo25DegUp(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 0)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 1)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 2)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 3)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 3, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 0)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 1)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 2)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 3)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 3, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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

        static void Track25DegUpToFlat(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 4)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 5)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 6)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 7)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 6, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 4)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 5)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 6)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE + 7)),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 6, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                }
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
            }
            else
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
        }

        static void Track25DegDown(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            Track25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void TrackFlatTo25DegDown(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            Track25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void Track25DegDownToFlat(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            TrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void TrackLeftQuarterTurn3(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 0)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 3)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 6)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 9)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 1)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 4)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 7)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 10)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 2)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 5)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 8)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE + 11)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackRightQuarterTurn3(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
            TrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
        }

        static void TrackLeftQuarterTurn5(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 0)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 5)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 10)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 15)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 1)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 6)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 11)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 16)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 2)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 7)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 12)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 17)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 3)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 8)),
                                { 0, 0, height }, { 16, 32, 3 }, { 0, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 13)),
                                { 0, 0, height }, { 16, 32, 3 }, { 0, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 18)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 4)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 9)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 14)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 19)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackRightQuarterTurn5(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
            TrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
        }

        static void TrackLeftEighthToDiag(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 0)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 4)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 8)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 12)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 1)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 5)),
                                { 0, 0, height }, { 34, 16, 3 }, { 0, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 9)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 13)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 2)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 6)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 10)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 14)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 4:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 3)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 7)),
                                { 0, 0, height }, { 16, 18, 3 }, { 0, 16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 11)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 15)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

        static void TrackRightEighthToDiag(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 16)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 20)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 24)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 28)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 17)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 21)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 25)),
                                { 0, 0, height }, { 34, 16, 3 }, { 0, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 29)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 18)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 22)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 26)),
                                { 0, 0, height }, { 28, 28, 3 }, { 4, 4, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 30)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 4:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 19)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 23)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 27)),
                                { 0, 0, height }, { 16, 18, 3 }, { 0, 16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE + 31)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

        static void TrackLeftEighthToOrthogonal(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
            TrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void TrackRightEighthToOrthogonal(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
            TrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
        }

        static void TrackDiagFlat(
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
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_FLAT_DIAGONAL + 3)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 3:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_FLAT_DIAGONAL + 3)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 1:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_FLAT_DIAGONAL + 0)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_FLAT_DIAGONAL + 0)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 2:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_FLAT_DIAGONAL + 2)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_FLAT_DIAGONAL + 2)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalASupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_LIFT_TRACK_FLAT_DIAGONAL + 1)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalASupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalASupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalASupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalASupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_FLAT_DIAGONAL + 1)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalASupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalASupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalASupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackDiag25DegUp(
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
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 11)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 3:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 11)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 1:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 8)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 8)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 2:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 10)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 10)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 3:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 1)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 1)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
            }
        }

        static void TrackDiag25DegUpToFlat(
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
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 7)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 3:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 7)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 1:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 4)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 4)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 2:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 6)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 6)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 3:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 5)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 5)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
            }
        }

        static void TrackDiagFlatTo25DegUp(
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
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 3)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 3:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 3)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 1:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 0)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 0)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 2:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 2)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 2)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 3:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 0, height + 2, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 1)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 0, height + 2, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 1)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
            }
        }

        static void TrackDiag25DegDown(
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
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 9)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 3:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 9)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 1:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 10)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 10)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 2:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 8)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 8)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 3:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 11)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 11)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
            }
        }

        static void TrackDiagFlatTo25DegDown(
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
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 5)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 3:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 5)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    break;
                case 1:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 6)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 6)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    break;
                case 2:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 4)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 4)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    break;
                case 3:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 7)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 7)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
        }

        static void TrackDiag25DegDownToFlat(
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
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 1)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 3:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 1)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 1:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 2)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 2)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 2:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 0)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 2:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 0)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 3:
                    if (trackElement.HasChain())
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex(
                                        (SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 3)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    else
                    {
                        switch (direction)
                        {
                            case 0:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 1:
                                PaintAddImageAsParentRotated(
                                    session, direction,
                                    session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 3)),
                                    { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 2:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                            case 3:
                                MetalBSupportsPaintSetup(
                                    session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                                break;
                        }
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
            }
        }

        static void TrackFlatToLeftBank(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 0)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 1)),
                        { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 2)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 3)),
                        { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 4)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 5)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                    session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
        }

        static void TrackFlatToRightBank(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 6)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 7)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 8)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 9)),
                        { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 10)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 11)),
                        { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                    session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
        }

        static void TrackLeftBankToFlat(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            TrackFlatToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void TrackRightBankToFlat(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            TrackFlatToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void TrackLeftBankTo25DegUp(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 12)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 13)),
                        { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 14)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 15)),
                        { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 16)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 17)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 3, height,
                    session.TrackColours[SCHEME_SUPPORTS]);
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

        static void TrackRightBankTo25DegUp(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 18)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 19)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 20)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 21)),
                        { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 22)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 23)),
                        { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 3, height,
                    session.TrackColours[SCHEME_SUPPORTS]);
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

        static void Track25DegUpToLeftBank(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 24)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 25)),
                        { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 26)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 27)),
                        { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 28)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 29)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 6, height,
                    session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
            }
            else
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
        }

        static void Track25DegUpToRightBank(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 30)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 31)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 32)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 33)),
                        { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 34)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 35)),
                        { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 6, height,
                    session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
            }
            else
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
        }

        static void TrackLeftBankTo25DegDown(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            Track25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void TrackRightBankTo25DegDown(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            Track25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void Track25DegDownToLeftBank(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            TrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void Track25DegDownToRightBank(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            TrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void TrackLeftBank(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 36)),
                        { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 37)),
                        { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 38)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 39)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                    session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
        }

        static void TrackRightBank(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            TrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void TrackDiagFlatToLeftBank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 4)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 0)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 1)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 27 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 3)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 2)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackDiagFlatToRightBank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 9)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 5)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 7)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 8)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 27 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 6)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackDiagLeftBankToFlat(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 6)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 7)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 8)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 27 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 5)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 9)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackDiagRightBankToFlat(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 2)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 3)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 0)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 1)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 27 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 4)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackDiagLeftBankTo25DegUp(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 14)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 10)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 11)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 35 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 13)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 12)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
            }
        }

        static void TrackDiagRightBankTo25DegUp(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 19)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 15)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 17)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 18)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 35 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 16)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
            }
        }

        static void TrackDiag25DegUpToLeftBank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 24)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 20)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 21)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 35 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 23)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 22)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
            }
        }

        static void TrackDiag25DegUpToRightBank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 29)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 25)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 27)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 28)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 35 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 26)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                    break;
            }
        }

        static void TrackDiagLeftBankTo25DegDown(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 26)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 27)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 28)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 35 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 25)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 29)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
        }

        static void TrackDiagRightBankTo25DegDown(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 22)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 23)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 20)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 21)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 35 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 24)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
        }

        static void TrackDiag25DegDownToLeftBank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 16)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 17)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 18)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 35 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 15)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 19)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
            }
        }

        static void TrackDiag25DegDownToRightBank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 12)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 13)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 10)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 11)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 35 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 14)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalBSupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
            }
        }

        static void TrackDiagLeftBank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 33)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 30)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 27 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 32)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 31)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackDiagRightBank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 31)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 32)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 2:
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 30)),
                                { -16, -16, height }, { 32, 32, 0 }, { -16, -16, height + 27 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex(
                                    (SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 33)),
                                { -16, -16, height }, { 32, 32, 3 }, { -16, -16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackLeftQuarterTurn3Bank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 0)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 1)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 4)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 7)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 11)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 2, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 2)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 5)),
                                { 0, 0, height }, { 16, 16, 1 }, { 0, 0, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 8)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 12)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 3)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 6)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 9)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 10)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 13)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 2, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackRightQuarterTurn3Bank(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
            TrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
        }

        static void TrackBankedLeftQuarterTurn5(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 0)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 1)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 6)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 11)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 17)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 1, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 2)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 7)),
                                { 0, 0, height }, { 32, 16, 1 }, { 0, 0, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 12)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 18)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 3)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 8)),
                                { 0, 0, height }, { 16, 16, 1 }, { 16, 16, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 13)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 19)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 4)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 9)),
                                { 0, 0, height }, { 16, 32, 1 }, { 0, 0, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 14)),
                                { 0, 0, height }, { 16, 32, 3 }, { 0, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 20)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 5)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 10)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 15)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 16)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 21)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 1, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackBankedRightQuarterTurn5(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
            TrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
        }

        static void TrackLeftEighthBankToDiag(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 0)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 4)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 8)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 12)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 1)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 5)),
                                { 0, 0, height }, { 34, 16, 0 }, { 0, 0, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 9)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 13)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 2)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 6)),
                                { 0, 0, height }, { 16, 16, 0 }, { 16, 16, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 10)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 14)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 4:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 3)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 7)),
                                { 0, 0, height }, { 16, 18, 0 }, { 0, 16, height + 27 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 11)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 15)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

        static void TrackRightEighthBankToDiag(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 16)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 20)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 24)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 28)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 17)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 21)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 25)),
                                { 0, 0, height }, { 34, 16, 0 }, { 0, 0, height + 27 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 29)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 18)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 22)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 26)),
                                { 0, 0, height }, { 28, 28, 0 }, { 4, 4, height + 27 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 30)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 4:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 19)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 23)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 27)),
                                { 0, 0, height }, { 16, 18, 0 }, { 0, 16, height + 27 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 31)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

        static void TrackLeftEighthBankToOrthogonal(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
            TrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
        }

        static void TrackRightEighthBankToOrthogonal(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
            TrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
        }

        static void TrackLeftQuarterTurn3Tile25DegUp(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 0)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 2)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 4)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 6)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 8, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 1)),
                                { 6, 0, height }, { 20, 32, 3 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 3)),
                                { 6, 0, height }, { 20, 32, 3 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 5)),
                                { 6, 0, height }, { 20, 32, 3 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 7)),
                                { 6, 0, height }, { 20, 32, 3 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 8, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                    break;
            }
        }

        static void TrackRightQuarterTurn3Tile25DegUp(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 8)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 10)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 12)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 14)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 8, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 9)),
                                { 6, 0, height }, { 20, 32, 3 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 11)),
                                { 6, 0, height }, { 20, 32, 3 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 13)),
                                { 6, 0, height }, { 20, 32, 3 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 15)),
                                { 6, 0, height }, { 20, 32, 3 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                    break;
            }
        }

        static void TrackLeftQuarterTurn3Tile25DegDown(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
            TrackRightQuarterTurn3Tile25DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
        }

        static void TrackRightQuarterTurn3Tile25DegDown(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
            TrackLeftQuarterTurn3Tile25DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
        }

        static void TrackLeftQuarterTurn525DegUp(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 0)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 5)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 10)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 15)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 8, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 1)),
                                { 0, 0, height }, { 32, 16, 3 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 6)),
                                { 0, 0, height }, { 32, 16, 3 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 11)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 16)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 2)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 7)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 12)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 17)),
                                { 0, 0, height }, { 16, 16, 3 });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 3)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 8)),
                                { 0, 0, height }, { 16, 32, 3 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 13)),
                                { 0, 0, height }, { 16, 32, 3 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 18)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 4)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 9)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 14)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 19)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 8, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                    break;
            }
        }

        static void TrackRightQuarterTurn525DegUp(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 20)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 25)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 30)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 35)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 8, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 21)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 26)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 31)),
                                { 0, 0, height }, { 32, 16, 3 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 36)),
                                { 0, 0, height }, { 32, 16, 3 });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 22)),
                                { 0, 0, height }, { 16, 16, 3 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 27)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 32)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 37)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 23)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 28)),
                                { 0, 0, height }, { 16, 32, 3 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 33)),
                                { 0, 0, height }, { 16, 32, 3 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 38)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 24)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 29)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 34)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 39)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 8, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                    break;
            }
        }

        static void TrackLeftQuarterTurn525DegDown(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
            TrackRightQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
        }

        static void TrackRightQuarterTurn525DegDown(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
            TrackLeftQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
        }

        static void TrackSBendLeft(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 0)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 4)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 3)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 7)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 1)),
                                { 0, 0, height }, { 32, 26, 3 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 5)),
                                { 0, 0, height }, { 32, 26, 3 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 6, 1, height - 1, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 2)),
                                { 0, 0, height }, { 32, 26, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 6)),
                                { 0, 0, height }, { 32, 26, 3 }, { 0, 6, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 2)),
                                { 0, 0, height }, { 32, 26, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 6)),
                                { 0, 0, height }, { 32, 26, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 1)),
                                { 0, 0, height }, { 32, 26, 3 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 5)),
                                { 0, 0, height }, { 32, 26, 3 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 6, 1, height - 1, session.TrackColours[SCHEME_SUPPORTS]);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 3)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 7)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 0)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 4)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackSBendRight(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 8)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 12)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 11)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 15)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 1:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 9)),
                                { 0, 0, height }, { 32, 26, 3 }, { 0, 6, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 13)),
                                { 0, 0, height }, { 32, 26, 3 }, { 0, 6, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 10)),
                                { 0, 0, height }, { 32, 26, 3 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 14)),
                                { 0, 0, height }, { 32, 26, 3 });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 10)),
                                { 0, 0, height }, { 32, 26, 3 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 14)),
                                { 0, 0, height }, { 32, 26, 3 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 9)),
                                { 0, 0, height }, { 32, 26, 3 }, { 0, 6, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 13)),
                                { 0, 0, height }, { 32, 26, 3 }, { 0, 6, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 11)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 15)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 8)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_S_BEND + 12)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackLeftHalfBankedHelixUpSmall(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 0)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 1)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 4)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 7)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 11)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 5, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 2)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 5)),
                                { 0, 0, height }, { 16, 16, 1 }, { 0, 0, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 8)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 12)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 3)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 6)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 9)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 10)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 13)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height + 8 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 9, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session,
                        PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 4:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 11)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 0)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 1)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 4)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 7)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 5, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session,
                        PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 12)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 2)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 5)),
                                { 0, 0, height }, { 16, 16, 1 }, { 0, 0, height + 27 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 8)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 7:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 13)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 8 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 3)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 6)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 9)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 10)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 9, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackRightHalfBankedHelixUpSmall(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 14)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 17)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 21)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 24)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 25)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 5, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 15)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 18)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 22)),
                                { 0, 0, height }, { 16, 16, 1 }, { 0, 0, height + 27 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 26)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 16)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height + 8 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 19)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 20)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 23)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 27)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 9, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session,
                        PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 4:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 17)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 21)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 24)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 25)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 14)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 5, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session,
                        PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 18)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 22)),
                                { 0, 0, height }, { 16, 16, 1 }, { 0, 0, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 26)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 15)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 7:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 19)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 20)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 23)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 27)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_SMALL_HELIX + 16)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 8 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 9, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackLeftHalfBankedHelixDownSmall(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            if (trackSequence >= 4)
            {
                trackSequence -= 4;
                direction = (direction - 1) & 3;
            }
            trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
            TrackRightHalfBankedHelixUpSmall(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
        }

        static void TrackRightHalfBankedHelixDownSmall(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            if (trackSequence >= 4)
            {
                trackSequence -= 4;
                direction = (direction + 1) & 3;
            }
            trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
            TrackLeftHalfBankedHelixUpSmall(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
        }

        static void TrackLeftHalfBankedHelixUpLarge(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 0)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 1)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 6)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 11)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 17)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 3, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 2)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 7)),
                                { 0, 0, height }, { 32, 16, 1 }, { 0, 0, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 12)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 18)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 3)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 8)),
                                { 0, 0, height }, { 16, 16, 1 }, { 16, 16, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 13)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 19)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 4)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 9)),
                                { 0, 0, height }, { 16, 32, 1 }, { 0, 0, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 14)),
                                { 0, 0, height }, { 16, 32, 3 }, { 0, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 20)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 5)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 10)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 15)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 16)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 21)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height + 8 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 9, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session,
                        PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 7:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 17)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 0)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 1)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 6)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 11)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 3, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session,
                        PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 18)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 2)),
                                { 0, 0, height }, { 16, 32, 3 }, { 0, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 7)),
                                { 0, 0, height }, { 16, 32, 1 }, { 0, 0, height + 27 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 12)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(
                            SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 10:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 19)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 3)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 8)),
                                { 0, 0, height }, { 16, 16, 1 }, { 16, 16, height + 27 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 13)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 20)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 4)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 9)),
                                { 0, 0, height }, { 32, 16, 1 }, { 0, 0, height + 27 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 14)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(
                            SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 13:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 21)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 8 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 5)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 10)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 15)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 16)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 9, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }
        static void TrackRightHalfBankedHelixUpLarge(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 22)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 27)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 33)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 38)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 39)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 3, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 23)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 28)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 34)),
                                { 0, 0, height }, { 32, 16, 1 }, { 0, 0, height + 27 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 40)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 24)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 29)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 35)),
                                { 0, 0, height }, { 16, 16, 1 }, { 16, 16, height + 27 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 41)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 25)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 30)),
                                { 0, 0, height }, { 16, 32, 3 }, { 0, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 36)),
                                { 0, 0, height }, { 16, 32, 1 }, { 0, 0, height + 27 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 42)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 26)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height + 8 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 31)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 32)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 4, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 37)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 43)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
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
                        session,
                        PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 7:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 27)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 33)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 38)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 39)),
                                { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 22)),
                                { 0, 0, height }, { 20, 32, 3 }, { 6, 0, height });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 3, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session,
                        PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 28)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 34)),
                                { 0, 0, height }, { 16, 32, 1 }, { 0, 0, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 40)),
                                { 0, 0, height }, { 16, 32, 3 }, { 0, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 23)),
                                { 0, 0, height }, { 16, 32, 3 }, { 16, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(
                            SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 10:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 29)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 16, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 35)),
                                { 0, 0, height }, { 16, 16, 1 }, { 16, 16, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 41)),
                                { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 24)),
                                { 0, 0, height }, { 16, 16, 3 }, { 0, 0, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                        0);
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 30)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 36)),
                                { 0, 0, height }, { 32, 16, 1 }, { 0, 0, height + 27 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 42)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 25)),
                                { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(
                            SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
                case 13:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 31)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 32)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 4, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 37)),
                                { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 43)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_LARGE_HELIX + 26)),
                                { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 8 });
                            MetalASupportsPaintSetup(
                                session, METAL_SUPPORTS_FORK_ALT, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session,
                        PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                        0xFFFF, 0);
                    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                    break;
            }
        }

        static void TrackLeftHalfBankedHelixDownLarge(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            if (trackSequence >= 7)
            {
                trackSequence -= 7;
                direction = (direction - 1) & 3;
            }
            trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
            TrackRightHalfBankedHelixUpLarge(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
        }

        static void TrackRightHalfBankedHelixDownLarge(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            if (trackSequence >= 7)
            {
                trackSequence -= 7;
                direction = (direction + 1) & 3;
            }
            trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
            TrackLeftHalfBankedHelixUpLarge(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
        }

        static void TrackBrakes(
            PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
            const TrackElement& trackElement)
        {
            switch (direction)
            {
                case 0:
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_FLAT + 0)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_FLAT + 1)),
                        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 0, height,
                    session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
        }

        static void TrackLeftBankToLeftQuarterTurn3Tile25DegUp(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 0)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 2)),
                                { 0, 6, height }, { 32, 20, 3 });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 3)),
                                { 0, 6, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 5)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 7)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 3, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                    break;
                case 1:
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 2:
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 1)),
                                { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 4)),
                                { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 6)),
                                { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 8)),
                                { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 10, height - 6,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    switch (direction)
                    {
                        case 2:
                            PaintUtilPushTunnelRight(session, height, TUNNEL_2);
                            break;
                        case 3:
                            PaintUtilPushTunnelLeft(session, height, TUNNEL_2);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                    break;
            }
        }

        static void TrackRightBankToRightQuarterTurn3Tile25DegUp(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 9)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 11)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 13)),
                                { 0, 6, height }, { 32, 20, 3 });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 14)),
                                { 0, 6, height }, { 32, 1, 26 }, { 0, 27, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 16)),
                                { 0, 6, height }, { 32, 20, 3 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 3, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                    break;
                case 1:
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 2:
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 10)),
                                { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 12)),
                                { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 15)),
                                { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 17)),
                                { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 10, height - 6,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    switch (direction)
                    {
                        case 0:
                            PaintUtilPushTunnelRight(session, height, TUNNEL_2);
                            break;
                        case 1:
                            PaintUtilPushTunnelLeft(session, height, TUNNEL_2);
                            break;
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                    break;
            }
        }

        static void TrackLeftQuarterTurn3Tile25DegDownToLeftBank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 12)),
                                { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 15)),
                                { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 17)),
                                { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 10)),
                                { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 8, height - 6,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                    break;
                case 1:
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 2:
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 11)),
                                { 6, 0, height }, { 20, 32, 3 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 13)),
                                { 6, 0, height }, { 20, 32, 3 });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 14)),
                                { 6, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 16)),
                                { 6, 0, height }, { 20, 32, 3 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 9)),
                                { 6, 0, height }, { 20, 32, 3 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 5, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                    break;
            }
        }

        static void TrackRightQuarterTurn3Tile25DegDownToRightBank(
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
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 8)),
                                { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 1)),
                                { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 4)),
                                { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 6)),
                                { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK_ALT : METAL_SUPPORTS_FORK, 4, 8, height - 6,
                        session.TrackColours[SCHEME_SUPPORTS]);
                    if (direction == 0 || direction == 3)
                    {
                        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
                    }
                    PaintUtilSetSegmentSupportHeight(
                        session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                    break;
                case 1:
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 2:
                    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                    break;
                case 3:
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 7)),
                                { 6, 0, height }, { 20, 32, 3 });
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 0)),
                                { 6, 0, height }, { 20, 32, 3 });
                            break;
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 2)),
                                { 6, 0, height }, { 20, 32, 3 });
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 3)),
                                { 6, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                            break;
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction,
                                session.TrackColours[SCHEME_TRACK].WithIndex((SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 5)),
                                { 6, 0, height }, { 20, 32, 3 });
                            break;
                    }
                    MetalASupportsPaintSetup(
                        session, direction & 1 ? METAL_SUPPORTS_FORK : METAL_SUPPORTS_FORK_ALT, 4, 5, height,
                        session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                        0);
                    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                    break;
            }
        }

        TRACK_PAINT_FUNCTION GetTrackPaintFunction(int32_t trackType)
        {
            switch (trackType)
            {
                case TrackElemType::Flat:
                    return TrackFlat;
                case TrackElemType::EndStation:
                case TrackElemType::BeginStation:
                case TrackElemType::MiddleStation:
                    return TrackStation;
                case TrackElemType::Up25:
                    return Track25DegUp;
                case TrackElemType::FlatToUp25:
                    return TrackFlatTo25DegUp;
                case TrackElemType::Up25ToFlat:
                    return Track25DegUpToFlat;
                case TrackElemType::Down25:
                    return Track25DegDown;
                case TrackElemType::FlatToDown25:
                    return TrackFlatTo25DegDown;
                case TrackElemType::Down25ToFlat:
                    return Track25DegDownToFlat;
                case TrackElemType::LeftQuarterTurn5Tiles:
                    return TrackLeftQuarterTurn5;
                case TrackElemType::RightQuarterTurn5Tiles:
                    return TrackRightQuarterTurn5;
                case TrackElemType::FlatToLeftBank:
                    return TrackFlatToLeftBank;
                case TrackElemType::FlatToRightBank:
                    return TrackFlatToRightBank;
                case TrackElemType::LeftBankToFlat:
                    return TrackLeftBankToFlat;
                case TrackElemType::RightBankToFlat:
                    return TrackRightBankToFlat;
                case TrackElemType::BankedLeftQuarterTurn5Tiles:
                    return TrackBankedLeftQuarterTurn5;
                case TrackElemType::BankedRightQuarterTurn5Tiles:
                    return TrackBankedRightQuarterTurn5;
                case TrackElemType::LeftBankToUp25:
                    return TrackLeftBankTo25DegUp;
                case TrackElemType::RightBankToUp25:
                    return TrackRightBankTo25DegUp;
                case TrackElemType::Up25ToLeftBank:
                    return Track25DegUpToLeftBank;
                case TrackElemType::Up25ToRightBank:
                    return Track25DegUpToRightBank;
                case TrackElemType::LeftBankToDown25:
                    return TrackLeftBankTo25DegDown;
                case TrackElemType::RightBankToDown25:
                    return TrackRightBankTo25DegDown;
                case TrackElemType::Down25ToLeftBank:
                    return Track25DegDownToLeftBank;
                case TrackElemType::Down25ToRightBank:
                    return Track25DegDownToRightBank;
                case TrackElemType::LeftBank:
                    return TrackLeftBank;
                case TrackElemType::RightBank:
                    return TrackRightBank;
                case TrackElemType::LeftQuarterTurn5TilesUp25:
                    return TrackLeftQuarterTurn525DegUp;
                case TrackElemType::RightQuarterTurn5TilesUp25:
                    return TrackRightQuarterTurn525DegUp;
                case TrackElemType::LeftQuarterTurn5TilesDown25:
                    return TrackLeftQuarterTurn525DegDown;
                case TrackElemType::RightQuarterTurn5TilesDown25:
                    return TrackRightQuarterTurn525DegDown;
                case TrackElemType::SBendLeft:
                    return TrackSBendLeft;
                case TrackElemType::SBendRight:
                    return TrackSBendRight;
                case TrackElemType::LeftQuarterTurn3Tiles:
                    return TrackLeftQuarterTurn3;
                case TrackElemType::RightQuarterTurn3Tiles:
                    return TrackRightQuarterTurn3;
                case TrackElemType::LeftBankedQuarterTurn3Tiles:
                    return TrackLeftQuarterTurn3Bank;
                case TrackElemType::RightBankedQuarterTurn3Tiles:
                    return TrackRightQuarterTurn3Bank;
                case TrackElemType::LeftQuarterTurn3TilesUp25:
                    return TrackLeftQuarterTurn3Tile25DegUp;
                case TrackElemType::RightQuarterTurn3TilesUp25:
                    return TrackRightQuarterTurn3Tile25DegUp;
                case TrackElemType::LeftQuarterTurn3TilesDown25:
                    return TrackLeftQuarterTurn3Tile25DegDown;
                case TrackElemType::RightQuarterTurn3TilesDown25:
                    return TrackRightQuarterTurn3Tile25DegDown;
                case TrackElemType::LeftHalfBankedHelixUpSmall:
                    return TrackLeftHalfBankedHelixUpSmall;
                case TrackElemType::RightHalfBankedHelixUpSmall:
                    return TrackRightHalfBankedHelixUpSmall;
                case TrackElemType::LeftHalfBankedHelixDownSmall:
                    return TrackLeftHalfBankedHelixDownSmall;
                case TrackElemType::RightHalfBankedHelixDownSmall:
                    return TrackRightHalfBankedHelixDownSmall;
                case TrackElemType::LeftHalfBankedHelixUpLarge:
                    return TrackLeftHalfBankedHelixUpLarge;
                case TrackElemType::RightHalfBankedHelixUpLarge:
                    return TrackRightHalfBankedHelixUpLarge;
                case TrackElemType::LeftHalfBankedHelixDownLarge:
                    return TrackLeftHalfBankedHelixDownLarge;
                case TrackElemType::RightHalfBankedHelixDownLarge:
                    return TrackRightHalfBankedHelixDownLarge;
                case TrackElemType::Brakes:
                    return TrackBrakes;
                case TrackElemType::LeftEighthToDiag:
                    return TrackLeftEighthToDiag;
                case TrackElemType::RightEighthToDiag:
                    return TrackRightEighthToDiag;
                case TrackElemType::LeftEighthToOrthogonal:
                    return TrackLeftEighthToOrthogonal;
                case TrackElemType::RightEighthToOrthogonal:
                    return TrackRightEighthToOrthogonal;
                case TrackElemType::LeftEighthBankToDiag:
                    return TrackLeftEighthBankToDiag;
                case TrackElemType::RightEighthBankToDiag:
                    return TrackRightEighthBankToDiag;
                case TrackElemType::LeftEighthBankToOrthogonal:
                    return TrackLeftEighthBankToOrthogonal;
                case TrackElemType::RightEighthBankToOrthogonal:
                    return TrackRightEighthBankToOrthogonal;
                case TrackElemType::DiagFlat:
                    return TrackDiagFlat;
                case TrackElemType::DiagUp25:
                    return TrackDiag25DegUp;
                case TrackElemType::DiagFlatToUp25:
                    return TrackDiagFlatTo25DegUp;
                case TrackElemType::DiagUp25ToFlat:
                    return TrackDiag25DegUpToFlat;
                case TrackElemType::DiagDown25:
                    return TrackDiag25DegDown;
                case TrackElemType::DiagFlatToDown25:
                    return TrackDiagFlatTo25DegDown;
                case TrackElemType::DiagDown25ToFlat:
                    return TrackDiag25DegDownToFlat;
                case TrackElemType::DiagFlatToLeftBank:
                    return TrackDiagFlatToLeftBank;
                case TrackElemType::DiagFlatToRightBank:
                    return TrackDiagFlatToRightBank;
                case TrackElemType::DiagLeftBankToFlat:
                    return TrackDiagLeftBankToFlat;
                case TrackElemType::DiagRightBankToFlat:
                    return TrackDiagRightBankToFlat;
                case TrackElemType::DiagLeftBankToUp25:
                    return TrackDiagLeftBankTo25DegUp;
                case TrackElemType::DiagRightBankToUp25:
                    return TrackDiagRightBankTo25DegUp;
                case TrackElemType::DiagUp25ToLeftBank:
                    return TrackDiag25DegUpToLeftBank;
                case TrackElemType::DiagUp25ToRightBank:
                    return TrackDiag25DegUpToRightBank;
                case TrackElemType::DiagLeftBankToDown25:
                    return TrackDiagLeftBankTo25DegDown;
                case TrackElemType::DiagRightBankToDown25:
                    return TrackDiagRightBankTo25DegDown;
                case TrackElemType::DiagDown25ToLeftBank:
                    return TrackDiag25DegDownToLeftBank;
                case TrackElemType::DiagDown25ToRightBank:
                    return TrackDiag25DegDownToRightBank;
                case TrackElemType::DiagLeftBank:
                    return TrackDiagLeftBank;
                case TrackElemType::DiagRightBank:
                    return TrackDiagRightBank;
                case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
                    return TrackLeftBankToLeftQuarterTurn3Tile25DegUp;
                case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
                    return TrackRightBankToRightQuarterTurn3Tile25DegUp;
                case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
                    return TrackLeftQuarterTurn3Tile25DegDownToLeftBank;
                case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
                    return TrackRightQuarterTurn3Tile25DegDownToRightBank;
            }
            return nullptr;
        }

    } // namespace AlpineRC
} // namespace OpenRCT2
