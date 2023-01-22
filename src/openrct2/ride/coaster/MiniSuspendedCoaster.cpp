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
namespace OpenRCT2
{
    /** rct2: 0x008AFE9C */
    static void MiniSuspendedRCTrackFlat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28433), { 0, 6, height + 24 },
                        { 32, 20, 1 });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28434), { 0, 6, height + 24 },
                        { 32, 20, 1 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28383), { 0, 6, height + 24 },
                        { 32, 20, 1 });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28384), { 0, 6, height + 24 },
                        { 32, 20, 1 });
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
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }

    /** rct2: 0x008AFF4C, 0x008AFF5C, 0x008AFF6C */
    static void MiniSuspendedRCTrackStation(
        PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { SPR_STATION_BASE_C_SW_NE, 28433, SPR_STATION_INVERTED_BAR_E_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 28434, SPR_STATION_INVERTED_BAR_E_NW_SE },
            { SPR_STATION_BASE_C_SW_NE, 28433, SPR_STATION_INVERTED_BAR_E_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 28434, SPR_STATION_INVERTED_BAR_E_NW_SE },
        };

        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][0]), { 0, 0, height },
            { 32, 28, 1 }, { 0, 2, height });
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]), { 0, 6, height + 24 },
            { 32, 20, 1 });
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours[SCHEME_SUPPORTS].WithIndex(imageIds[direction][2]), { 0, 6, height + 24 },
            { 32, 20, 1 }, { 0, 2, height });
        TrackPaintUtilDrawStationMetalSupports2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 3);
        TrackPaintUtilDrawStationInverted(session, ride, direction, height, trackElement, STATION_VARIANT_1);
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    /** rct2: 0x008AFEAC */
    static void MiniSuspendedRCTrack25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28435), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 40 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28441), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 40 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28437), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 40 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28442), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 40 });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28385), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 40 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28391), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 40 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28387), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 40 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28392), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_FORK, 6, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK_ALT, 8, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK, 7, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK_ALT, 5, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
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
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }

    /** rct2: 0x008AFEBC */
    static void MiniSuspendedRCTrackFlatTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28436), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28443), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28438), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28444), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28386), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28393), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28388), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28394), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_FORK, 6, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK_ALT, 8, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK, 7, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK_ALT, 5, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
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
        PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
    }

    /** rct2: 0x008AFECC */
    static void MiniSuspendedRCTrack25DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28440), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28446), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28439), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28445), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28390), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28396), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28389), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28395), { 0, 6, height + 24 },
                        { 32, 20, 1 }, { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_FORK, 6, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK_ALT, 8, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK, 7, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_FORK_ALT, 5, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
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
        PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
    }

    /** rct2: 0x008AFEDC */
    static void MiniSuspendedRCTrack25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        MiniSuspendedRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x008AFEEC */
    static void MiniSuspendedRCTrackFlatTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        MiniSuspendedRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x008AFEFC */
    static void MiniSuspendedRCTrack25DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        MiniSuspendedRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x008AFF0C */
    static void MiniSuspendedRCTrackLeftQuarterTurn5(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28406), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28411), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28416), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28401), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28405), { 0, 0, height + 24 },
                            { 32, 16, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28410), { 0, 0, height + 24 },
                            { 32, 16, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28415), { 0, 16, height + 24 },
                            { 32, 16, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28400), { 0, 16, height + 24 },
                            { 32, 16, 1 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28404), { 0, 16, height + 24 },
                            { 16, 16, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28409), { 16, 16, height + 24 },
                            { 16, 16, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28414), { 16, 0, height + 24 },
                            { 16, 16, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28399), { 0, 0, height + 24 },
                            { 16, 16, 1 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28403), { 16, 0, height + 24 },
                            { 16, 32, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28408), { 0, 0, height + 24 },
                            { 16, 32, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28413), { 0, 0, height + 24 },
                            { 16, 32, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28398), { 16, 0, height + 24 },
                            { 16, 32, 1 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28402), { 6, 0, height + 24 },
                            { 20, 32, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28407), { 6, 0, height + 24 },
                            { 20, 32, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28412), { 6, 0, height + 24 },
                            { 20, 32, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28397), { 6, 0, height + 24 },
                            { 20, 32, 1 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x008AFF1C */
    static void MiniSuspendedRCTrackRightQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        MiniSuspendedRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
    }

    /** rct2: 0x008AFF2C */
    static void MiniSuspendedRCTrackSBendLeft(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28421), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28428), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28424), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28425), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28422), { 0, 0, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28427), { 0, 0, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28423), { 0, 6, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28426), { 0, 6, height + 24 },
                            { 32, 26, 1 });
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
                            session, METAL_SUPPORTS_FORK, 5, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 6, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28423), { 0, 6, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28426), { 0, 6, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28422), { 0, 0, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28427), { 0, 0, height + 24 },
                            { 32, 26, 1 });
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
                            session, METAL_SUPPORTS_FORK, 5, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 6, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28424), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28425), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28421), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28428), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x008AFF3C */
    static void MiniSuspendedRCTrackSBendRight(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28417), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28432), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28420), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28429), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28418), { 0, 6, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28431), { 0, 6, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28419), { 0, 0, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28430), { 0, 0, height + 24 },
                            { 32, 26, 1 });
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
                            session, METAL_SUPPORTS_FORK, 8, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 7, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28419), { 0, 0, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28430), { 0, 0, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28418), { 0, 6, height + 24 },
                            { 32, 26, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28431), { 0, 6, height + 24 },
                            { 32, 26, 1 });
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
                            session, METAL_SUPPORTS_FORK, 8, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 7, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28420), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28429), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28417), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28432), { 0, 6, height + 24 },
                            { 32, 20, 1 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x008AFF7C */
    static void MiniSuspendedRCTrackLeftQuarterTurn3(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28452), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28455), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28458), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28449), { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28451), { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28454), { 0, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28457), { 0, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28448), { 16, 16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28450), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28453), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28456), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28447), { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

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

    /** rct2: 0x008AFF8C */
    static void MiniSuspendedRCTrackRightQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        MiniSuspendedRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
    }

    /** rct2: 0x008AFFAC */
    static void MiniSuspendedRCTrackLeftEighthToDiag(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28475), { 0, 0, height + 24 },
                            { 32, 20, 1 }, { 0, 6, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28479), { 0, 0, height + 24 },
                            { 32, 20, 1 }, { 0, 6, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28483), { 0, 0, height + 24 },
                            { 32, 20, 1 }, { 0, 6, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28487), { 0, 0, height + 24 },
                            { 32, 20, 1 }, { 0, 6, height + 24 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28476), { 0, 0, height + 24 },
                            { 32, 16, 1 }, { 0, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28480), { 0, 0, height + 24 },
                            { 34, 16, 1 }, { 0, 0, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28484), { 0, 0, height + 24 },
                            { 32, 16, 1 }, { 0, 16, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28488), { 0, 0, height + 24 },
                            { 32, 16, 1 }, { 0, 16, height + 24 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28477), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 0, 16, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28481), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 16, 16, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28485), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 16, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28489), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 0, 0, height + 24 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28478), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 16, 16, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28482), { 0, 0, height + 24 },
                            { 16, 18, 1 }, { 0, 16, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28486), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 0, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28490), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 16, 0, height + 24 });
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
                            session, METAL_SUPPORTS_FORK, 3, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 1, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 0, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 2, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x008AFFBC */
    static void MiniSuspendedRCTrackRightEighthToDiag(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28459), { 0, 0, height + 24 },
                            { 32, 20, 1 }, { 0, 6, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28463), { 0, 0, height + 24 },
                            { 32, 20, 1 }, { 0, 6, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28467), { 0, 0, height + 24 },
                            { 32, 20, 1 }, { 0, 6, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28471), { 0, 0, height + 24 },
                            { 32, 20, 1 }, { 0, 6, height + 24 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28460), { 0, 0, height + 24 },
                            { 32, 16, 1 }, { 0, 16, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28464), { 0, 0, height + 24 },
                            { 32, 16, 1 }, { 0, 16, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28468), { 0, 0, height + 24 },
                            { 34, 16, 1 }, { 0, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28472), { 0, 0, height + 24 },
                            { 32, 16, 1 }, { 0, 0, height + 24 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28461), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 0, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28465), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 16, 0, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28469), { 0, 0, height + 24 },
                            { 28, 28, 1 }, { 4, 2, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28473), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 0, 16, height + 24 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28462), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 16, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28466), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 0, 0, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28470), { 0, 0, height + 24 },
                            { 16, 18, 1 }, { 0, 16, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28474), { 0, 0, height + 24 },
                            { 16, 16, 1 }, { 16, 16, height + 24 });
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
                            session, METAL_SUPPORTS_FORK, 1, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 0, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 2, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 3, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x008AFFCC */
    static void MiniSuspendedRCTrackLeftEighthToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        MiniSuspendedRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x008AFFDC */
    static void MiniSuspendedRCTrackRightEighthToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        MiniSuspendedRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
    }

    /** rct2: 0x008AFF9C */
    static void MiniSuspendedRCTrackDiagFlat(
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28510),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 24 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28494),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 24 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28507),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 24 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28491),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 24 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28509),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 24 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28493),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 24 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28508),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 24 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28492),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 24 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 1, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 0, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 2, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 3, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x008B000C */
    static void MiniSuspendedRCTrackDiag25DegUp(
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28522),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28506),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28519),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28503),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28521),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28505),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28520),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28504),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }

    /** rct2: 0x008AFFEC */
    static void MiniSuspendedRCTrackDiagFlatTo25DegUp(
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28514),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28498),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28511),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28495),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28513),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28497),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28512),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28496),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 1, 0, height + 34, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 0, 0, height + 34, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 2, 0, height + 34, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 3, 0, height + 34, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
        }
    }

    /** rct2: 0x008AFFFC */
    static void MiniSuspendedRCTrackDiag25DegUpToFlat(
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28518),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28502),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28515),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28499),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28517),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28501),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28516),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28500),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 1, 0, height + 36, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 0, 0, height + 36, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 2, 0, height + 36, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 3, 0, height + 36, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }

    /** rct2: 0x008B003C */
    static void MiniSuspendedRCTrackDiag25DegDown(
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28520),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28504),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28521),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28505),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28519),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28503),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28522),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28506),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 40 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }

    /** rct2: 0x008B001C */
    static void MiniSuspendedRCTrackDiagFlatTo25DegDown(
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28516),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28500),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28517),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28501),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28515),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28499),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28518),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28502),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 1, 0, height + 36, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 0, 0, height + 36, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 2, 0, height + 36, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 3, 0, height + 36, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                break;
        }

        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }

    /** rct2: 0x008B002C */
    static void MiniSuspendedRCTrackDiag25DegDownToFlat(
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28512),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28496),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28513),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28497),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28511),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28495),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
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
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28514),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(28498),
                                { -16, -16, height + 24 }, { 32, 32, 1 }, { -16, -16, height + 32 });
                            break;
                    }
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 1, 0, height + 34, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 0, 0, height + 34, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK, 2, 0, height + 34, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_FORK_ALT, 3, 0, height + 34, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
        }
    }

    TRACK_PAINT_FUNCTION GetTrackPaintFunctionMiniSuspendedRC(int32_t trackType)
    {
        switch (trackType)
        {
            case TrackElemType::Flat:
                return MiniSuspendedRCTrackFlat;
            case TrackElemType::EndStation:
            case TrackElemType::BeginStation:
            case TrackElemType::MiddleStation:
                return MiniSuspendedRCTrackStation;
            case TrackElemType::Up25:
                return MiniSuspendedRCTrack25DegUp;
            case TrackElemType::FlatToUp25:
                return MiniSuspendedRCTrackFlatTo25DegUp;
            case TrackElemType::Up25ToFlat:
                return MiniSuspendedRCTrack25DegUpToFlat;
            case TrackElemType::Down25:
                return MiniSuspendedRCTrack25DegDown;
            case TrackElemType::FlatToDown25:
                return MiniSuspendedRCTrackFlatTo25DegDown;
            case TrackElemType::Down25ToFlat:
                return MiniSuspendedRCTrack25DegDownToFlat;
            case TrackElemType::LeftQuarterTurn5Tiles:
                return MiniSuspendedRCTrackLeftQuarterTurn5;
            case TrackElemType::RightQuarterTurn5Tiles:
                return MiniSuspendedRCTrackRightQuarterTurn5;
            case TrackElemType::SBendLeft:
                return MiniSuspendedRCTrackSBendLeft;
            case TrackElemType::SBendRight:
                return MiniSuspendedRCTrackSBendRight;
            case TrackElemType::LeftQuarterTurn3Tiles:
                return MiniSuspendedRCTrackLeftQuarterTurn3;
            case TrackElemType::RightQuarterTurn3Tiles:
                return MiniSuspendedRCTrackRightQuarterTurn3;
            case TrackElemType::LeftEighthToDiag:
                return MiniSuspendedRCTrackLeftEighthToDiag;
            case TrackElemType::RightEighthToDiag:
                return MiniSuspendedRCTrackRightEighthToDiag;
            case TrackElemType::LeftEighthToOrthogonal:
                return MiniSuspendedRCTrackLeftEighthToOrthogonal;
            case TrackElemType::RightEighthToOrthogonal:
                return MiniSuspendedRCTrackRightEighthToOrthogonal;
            case TrackElemType::DiagFlat:
                return MiniSuspendedRCTrackDiagFlat;
            case TrackElemType::DiagUp25:
                return MiniSuspendedRCTrackDiag25DegUp;
            case TrackElemType::DiagFlatToUp25:
                return MiniSuspendedRCTrackDiagFlatTo25DegUp;
            case TrackElemType::DiagUp25ToFlat:
                return MiniSuspendedRCTrackDiag25DegUpToFlat;
            case TrackElemType::DiagDown25:
                return MiniSuspendedRCTrackDiag25DegDown;
            case TrackElemType::DiagFlatToDown25:
                return MiniSuspendedRCTrackDiagFlatTo25DegDown;
            case TrackElemType::DiagDown25ToFlat:
                return MiniSuspendedRCTrackDiag25DegDownToFlat;
        }
        return nullptr;
    }
} // namespace OpenRCT2
