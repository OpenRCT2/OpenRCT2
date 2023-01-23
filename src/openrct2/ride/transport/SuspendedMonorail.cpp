/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../entity/EntityRegistry.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.TileElement.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"
namespace OpenRCT2
{
    /** rct2: 0x008636F4 */
    static void SuspendedMonorailTrackFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25853), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25854), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }

    /** rct2: 0x008637A4, 0x008637B4, 0x008637C4 */
    static void SuspendedMonorailTrackStation(
        PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { SPR_STATION_BASE_C_SW_NE, 25853, SPR_STATION_INVERTED_BAR_F_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 25854, SPR_STATION_INVERTED_BAR_F_NW_SE },
            { SPR_STATION_BASE_C_SW_NE, 25853, SPR_STATION_INVERTED_BAR_F_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 25854, SPR_STATION_INVERTED_BAR_F_NW_SE },
        };

        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][0]), { 0, 0, height },
            { 32, 28, 1 }, { 0, 2, height });
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]), { 0, 0, height + 32 },
            { 32, 20, 3 }, { 0, 6, height + 32 });
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours[SCHEME_SUPPORTS].WithIndex(imageIds[direction][2]), { 0, 6, height + 32 },
            { 32, 20, 3 }, { 0, 6, height + 32 });
        TrackPaintUtilDrawStationMetalSupports2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 3);
        TrackPaintUtilDrawStationInverted(session, ride, direction, height, trackElement, STATION_VARIANT_TALL);
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }

    /** rct2: 0x00863704 */
    static void SuspendedMonorailTrack25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25945), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 48 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25946), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 48 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25947), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 48 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25948), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 48 });
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
                        session, METAL_SUPPORTS_BOXED, 6, 0, height + 60, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 8, 0, height + 60, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 7, 0, height + 60, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 5, 0, height + 60, session.TrackColours[SCHEME_SUPPORTS]);
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

    /** rct2: 0x00863714 */
    static void SuspendedMonorailTrackFlatTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25937), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25938), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25939), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25940), { 0, 0, height + 32 },
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
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
        }
        PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
    }

    /** rct2: 0x00863724 */
    static void SuspendedMonorailTrack25DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25941), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25942), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25943), { 0, 0, height + 32 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25944), { 0, 0, height + 32 },
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
                        session, METAL_SUPPORTS_BOXED, 6, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 8, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 7, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_BOXED, 5, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_INVERTED_9);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_15);
        }
        PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
    }

    /** rct2: 0x00863734 */
    static void SuspendedMonorailTrack25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        SuspendedMonorailTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x00863744 */
    static void SuspendedMonorailTrackFlatTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        SuspendedMonorailTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x00863754 */
    static void SuspendedMonorailTrack25DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        SuspendedMonorailTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x00863764 */
    static void SuspendedMonorailTrackLeftQuarterTurn5(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25876), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25881), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25886), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25871), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);

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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25875), { 0, 0, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25880), { 0, 0, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25885), { 0, 0, height + 32 },
                            { 32, 16, 3 }, { 0, 16, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25870), { 0, 0, height + 32 },
                            { 32, 16, 3 }, { 0, 16, height + 32 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25874), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 0, 16, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25879), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 16, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25884), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 0, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25869), { 0, 0, height + 32 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25873), { 0, 0, height + 32 },
                            { 16, 32, 3 }, { 16, 0, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25878), { 0, 0, height + 32 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25883), { 0, 0, height + 32 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25868), { 0, 0, height + 32 },
                            { 16, 32, 3 }, { 16, 0, height + 32 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25872), { 0, 0, height + 32 },
                            { 20, 32, 3 }, { 6, 0, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25877), { 0, 0, height + 32 },
                            { 20, 32, 3 }, { 6, 0, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25882), { 0, 0, height + 32 },
                            { 20, 32, 3 }, { 6, 0, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25867), { 0, 0, height + 32 },
                            { 20, 32, 3 }, { 6, 0, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);

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

    /** rct2: 0x00863774 */
    static void SuspendedMonorailTrackRightQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        SuspendedMonorailTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
    }

    /** rct2: 0x00863784 */
    static void SuspendedMonorailTrackSBendLeft(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25919), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25923), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25922), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25926), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25920), { 0, 0, height + 32 },
                            { 32, 26, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25924), { 0, 0, height + 32 },
                            { 32, 26, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25921), { 0, 0, height + 32 },
                            { 32, 26, 3 }, { 0, 6, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25925), { 0, 0, height + 32 },
                            { 32, 26, 3 }, { 0, 6, height + 32 });
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
                            session, METAL_SUPPORTS_BOXED, 5, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 6, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25921), { 0, 0, height + 32 },
                            { 32, 26, 3 }, { 0, 6, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25925), { 0, 0, height + 32 },
                            { 32, 26, 3 }, { 0, 6, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25920), { 0, 0, height + 32 },
                            { 32, 26, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25924), { 0, 0, height + 32 },
                            { 32, 26, 3 });
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
                            session, METAL_SUPPORTS_BOXED, 5, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 6, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25922), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25926), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25919), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25923), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_INVERTED_9);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_INVERTED_9);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x00863794 */
    static void SuspendedMonorailTrackSBendRight(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25927), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25931), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25930), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25934), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25928), { 0, 0, height + 32 },
                            { 32, 26, 3 }, { 0, 6, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25932), { 0, 0, height + 32 },
                            { 32, 26, 3 }, { 0, 6, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25929), { 0, 0, height + 32 },
                            { 32, 26, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25933), { 0, 0, height + 32 },
                            { 32, 26, 3 });
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
                            session, METAL_SUPPORTS_BOXED, 8, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 7, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25929), { 0, 0, height + 32 },
                            { 32, 26, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25933), { 0, 0, height + 32 },
                            { 32, 26, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25928), { 0, 0, height + 32 },
                            { 32, 26, 3 }, { 0, 6, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25932), { 0, 0, height + 32 },
                            { 32, 26, 3 }, { 0, 6, height + 32 });
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
                            session, METAL_SUPPORTS_BOXED, 8, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 7, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25930), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25934), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25927), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25931), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_INVERTED_9);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_INVERTED_9);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x008637D4 */
    static void SuspendedMonorailTrackLeftQuarterTurn3(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25860), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25863), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25866), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25857), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);

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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25859), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 0, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25862), { 0, 0, height + 32 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25865), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 0, 16, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25856), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25858), { 0, 0, height + 32 },
                            { 20, 32, 3 }, { 6, 0, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25861), { 0, 0, height + 32 },
                            { 20, 32, 3 }, { 6, 0, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25864), { 0, 0, height + 32 },
                            { 20, 32, 3 }, { 6, 0, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25855), { 0, 0, height + 32 },
                            { 20, 32, 3 }, { 6, 0, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);

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

    /** rct2: 0x008637E4 */
    static void SuspendedMonorailTrackRightQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        SuspendedMonorailTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
    }

    /** rct2: 0x00863864 */
    static void SuspendedMonorailTrackLeftEighthToDiag(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25903), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25907), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25911), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25915), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25904), { 0, 0, height + 32 },
                            { 32, 16, 3 }, { 0, 0, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25908), { 0, 0, height + 32 },
                            { 34, 16, 3 }, { 0, 0, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25912), { 0, 0, height + 32 },
                            { 32, 16, 3 }, { 0, 16, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25916), { 0, 0, height + 32 },
                            { 32, 16, 3 }, { 0, 16, height + 32 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25905), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 0, 16, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25909), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 16, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25913), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 0, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25917), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 0, 0, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25906), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 16, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25910), { 0, 0, height + 32 },
                            { 16, 18, 3 }, { 0, 16, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25914), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 0, 0, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25918), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 0, height + 32 });
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
                            session, METAL_SUPPORTS_BOXED, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x00863874 */
    static void SuspendedMonorailTrackRightEighthToDiag(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25887), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25891), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25895), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25899), { 0, 0, height + 32 },
                            { 32, 20, 3 }, { 0, 6, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25888), { 0, 0, height + 32 },
                            { 32, 16, 3 }, { 0, 16, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25892), { 0, 0, height + 32 },
                            { 32, 16, 3 }, { 0, 16, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25896), { 0, 0, height + 32 },
                            { 34, 16, 3 }, { 0, 0, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25900), { 0, 0, height + 32 },
                            { 32, 16, 3 }, { 0, 0, height + 32 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25889), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 0, 0, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25893), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 0, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25897), { 0, 0, height + 32 },
                            { 28, 28, 3 }, { 4, 4, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25901), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 0, 16, height + 32 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25890), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 0, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25894), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 0, 0, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25898), { 0, 0, height + 32 },
                            { 16, 18, 3 }, { 0, 16, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25902), { 0, 0, height + 32 },
                            { 16, 16, 3 }, { 16, 16, height + 32 });
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
                            session, METAL_SUPPORTS_BOXED, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x00863884 */
    static void SuspendedMonorailTrackLeftEighthToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        SuspendedMonorailTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x00863894 */
    static void SuspendedMonorailTrackRightEighthToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        SuspendedMonorailTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
    }

    /** rct2: 0x008637F4 */
    static void SuspendedMonorailTrackDiagFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25936), { -16, -16, height + 32 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25935), { -16, -16, height + 32 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25935), { -16, -16, height + 32 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25936), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x00863824 */
    static void SuspendedMonorailTrackDiag25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25960), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 48 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25957), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 48 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25959), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 48 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25958), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 48 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 1, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 0, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 2, 0, height + 55, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 3, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }

    /** rct2: 0x00863804 */
    static void SuspendedMonorailTrackDiagFlatTo25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25952), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25949), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25951), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25950), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, METAL_SUPPORTS_BOXED, 3, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
        }
    }

    /** rct2: 0x00863814 */
    static void SuspendedMonorailTrackDiag25DegUpToFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25956), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25953), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25955), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25954), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 1, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 0, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 2, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 3, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }

    /** rct2: 0x00863854 */
    static void SuspendedMonorailTrackDiag25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25958), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 48 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25959), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 48 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25957), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 48 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25960), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 48 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 1, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 0, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 2, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 3, 0, height + 58, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }

    /** rct2: 0x00863834 */
    static void SuspendedMonorailTrackDiagFlatTo25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25954), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25955), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25953), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25956), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                break;
        }

        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }

    /** rct2: 0x00863844 */
    static void SuspendedMonorailTrackDiag25DegDownToFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25950), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25951), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25949), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(25952), { -16, -16, height + 32 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 1, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 0, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 2, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_BOXED, 3, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
        }
    }

    TRACK_PAINT_FUNCTION GetTrackPaintFunctionSuspendedMonorail(int32_t trackType)
    {
        switch (trackType)
        {
            case TrackElemType::Flat:
                return SuspendedMonorailTrackFlat;
            case TrackElemType::EndStation:
            case TrackElemType::BeginStation:
            case TrackElemType::MiddleStation:
                return SuspendedMonorailTrackStation;
            case TrackElemType::Up25:
                return SuspendedMonorailTrack25DegUp;
            case TrackElemType::FlatToUp25:
                return SuspendedMonorailTrackFlatTo25DegUp;
            case TrackElemType::Up25ToFlat:
                return SuspendedMonorailTrack25DegUpToFlat;
            case TrackElemType::Down25:
                return SuspendedMonorailTrack25DegDown;
            case TrackElemType::FlatToDown25:
                return SuspendedMonorailTrackFlatTo25DegDown;
            case TrackElemType::Down25ToFlat:
                return SuspendedMonorailTrack25DegDownToFlat;
            case TrackElemType::LeftQuarterTurn5Tiles:
                return SuspendedMonorailTrackLeftQuarterTurn5;
            case TrackElemType::RightQuarterTurn5Tiles:
                return SuspendedMonorailTrackRightQuarterTurn5;
            case TrackElemType::SBendLeft:
                return SuspendedMonorailTrackSBendLeft;
            case TrackElemType::SBendRight:
                return SuspendedMonorailTrackSBendRight;
            case TrackElemType::LeftQuarterTurn3Tiles:
                return SuspendedMonorailTrackLeftQuarterTurn3;
            case TrackElemType::RightQuarterTurn3Tiles:
                return SuspendedMonorailTrackRightQuarterTurn3;
            case TrackElemType::LeftEighthToDiag:
                return SuspendedMonorailTrackLeftEighthToDiag;
            case TrackElemType::RightEighthToDiag:
                return SuspendedMonorailTrackRightEighthToDiag;
            case TrackElemType::LeftEighthToOrthogonal:
                return SuspendedMonorailTrackLeftEighthToOrthogonal;
            case TrackElemType::RightEighthToOrthogonal:
                return SuspendedMonorailTrackRightEighthToOrthogonal;
            case TrackElemType::DiagFlat:
                return SuspendedMonorailTrackDiagFlat;
            case TrackElemType::DiagUp25:
                return SuspendedMonorailTrackDiag25DegUp;
            case TrackElemType::DiagFlatToUp25:
                return SuspendedMonorailTrackDiagFlatTo25DegUp;
            case TrackElemType::DiagUp25ToFlat:
                return SuspendedMonorailTrackDiag25DegUpToFlat;
            case TrackElemType::DiagDown25:
                return SuspendedMonorailTrackDiag25DegDown;
            case TrackElemType::DiagFlatToDown25:
                return SuspendedMonorailTrackDiagFlatTo25DegDown;
            case TrackElemType::DiagDown25ToFlat:
                return SuspendedMonorailTrackDiag25DegDownToFlat;
        }
        return nullptr;
    }
} // namespace OpenRCT2
