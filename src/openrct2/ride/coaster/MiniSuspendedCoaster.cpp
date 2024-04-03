/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/support/MetalSupports.h"
#include "../../paint/tile_element/Paint.TileElement.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"

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
                    session, direction, session.TrackColours.WithIndex(28433), { 0, 6, height + 24 }, { 32, 20, 1 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28434), { 0, 6, height + 24 }, { 32, 20, 1 });
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
                    session, direction, session.TrackColours.WithIndex(28383), { 0, 6, height + 24 }, { 32, 20, 1 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28384), { 0, 6, height + 24 }, { 32, 20, 1 });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide), direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                break;
            case 1:
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
    static constexpr uint32_t imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 28433, SPR_STATION_INVERTED_BAR_E_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 28434, SPR_STATION_INVERTED_BAR_E_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 28433, SPR_STATION_INVERTED_BAR_E_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 28434, SPR_STATION_INVERTED_BAR_E_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][0]), { 0, 0, height },
        { { 0, 2, height }, { 32, 28, 1 } });
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(imageIds[direction][1]), { 0, 6, height + 24 }, { 32, 20, 1 });
    PaintAddImageAsChildRotated(
        session, direction, session.SupportColours.WithIndex(imageIds[direction][2]), { 0, 6, height + 24 },
        { { 0, 2, height }, { 32, 20, 1 } });
    DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Boxed);
    TrackPaintUtilDrawStationInverted(session, ride, direction, height, trackElement, STATION_VARIANT_1);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
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
                    session, direction, session.TrackColours.WithIndex(28435), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28441), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28437), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28442), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28385), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28391), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28387), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28392), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide), direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Fork, MetalSupportPlace::TopRightSide, 0, height + 46, session.SupportColours);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomRightSide, 0, height + 46,
                    session.SupportColours);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Fork, MetalSupportPlace::BottomLeftSide, 0, height + 46, session.SupportColours);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::ForkAlt, MetalSupportPlace::TopLeftSide, 0, height + 46, session.SupportColours);
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
                    session, direction, session.TrackColours.WithIndex(28436), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28443), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28438), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28444), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28386), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28393), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28388), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28394), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide), direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Fork, MetalSupportPlace::TopRightSide, 0, height + 38, session.SupportColours);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomRightSide, 0, height + 38,
                    session.SupportColours);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Fork, MetalSupportPlace::BottomLeftSide, 0, height + 38, session.SupportColours);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::ForkAlt, MetalSupportPlace::TopLeftSide, 0, height + 38, session.SupportColours);
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
                    session, direction, session.TrackColours.WithIndex(28440), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28446), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28439), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28445), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28390), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28396), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28389), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28395), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide), direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Fork, MetalSupportPlace::TopRightSide, 0, height + 38, session.SupportColours);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomRightSide, 0, height + 38,
                    session.SupportColours);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::Fork, MetalSupportPlace::BottomLeftSide, 0, height + 38, session.SupportColours);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportType::ForkAlt, MetalSupportPlace::TopLeftSide, 0, height + 38, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28406), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28411), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28416), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28401), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28405), { 0, 0, height + 24 }, { 32, 16, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28410), { 0, 0, height + 24 }, { 32, 16, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28415), { 0, 16, height + 24 }, { 32, 16, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28400), { 0, 16, height + 24 }, { 32, 16, 1 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28404), { 0, 16, height + 24 }, { 16, 16, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28409), { 16, 16, height + 24 }, { 16, 16, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28414), { 16, 0, height + 24 }, { 16, 16, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28399), { 0, 0, height + 24 }, { 16, 16, 1 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
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
                        session, direction, session.TrackColours.WithIndex(28403), { 16, 0, height + 24 }, { 16, 32, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28408), { 0, 0, height + 24 }, { 16, 32, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28413), { 0, 0, height + 24 }, { 16, 32, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28398), { 16, 0, height + 24 }, { 16, 32, 1 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28402), { 6, 0, height + 24 }, { 20, 32, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28407), { 6, 0, height + 24 }, { 20, 32, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28412), { 6, 0, height + 24 }, { 20, 32, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28397), { 6, 0, height + 24 }, { 20, 32, 1 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28421), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28428), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28424), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28425), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28422), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28427), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28423), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28426), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::TopLeftSide, 0, height + 30,
                        session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::TopRightSide, 0, height + 30,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28423), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28426), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28422), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28427), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::TopLeftSide, 0, height + 30,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::TopRightSide, 0, height + 30,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28424), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28425), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28421), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28428), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28417), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28432), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28420), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28429), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28418), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28431), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28419), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28430), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::BottomRightSide, 0, height + 30,
                        session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomLeftSide, 0, height + 30,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28419), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28430), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28418), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28431), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::BottomRightSide, 0, height + 30,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomLeftSide, 0, height + 30,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28420), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28429), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28417), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28432), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28452), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28455), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28458), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28449), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28451), { 16, 0, height + 24 }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28454), { 0, 0, height + 24 }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28457), { 0, 16, height + 24 }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28448), { 16, 16, height + 24 }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28450), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28453), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28456), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28447), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28475), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28479), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28483), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28487), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28476), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 32, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28480), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 34, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28484), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28488), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 32, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28477), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28481), { 0, 0, height + 24 },
                        { { 16, 16, height + 24 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28485), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28489), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28478), { 0, 0, height + 24 },
                        { { 16, 16, height + 24 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28482), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 16, 18, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28486), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28490), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 16, 1 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::BottomCorner, 0, height + 30,
                        session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::LeftCorner, 0, height + 30,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::TopCorner, 0, height + 30, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::RightCorner, 0, height + 30,
                        session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28459), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28463), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28467), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28471), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::Centre, 0, height + 30, session.SupportColours);
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
                        session, direction, session.TrackColours.WithIndex(28460), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 32, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28464), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28468), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 34, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28472), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 32, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28461), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28465), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28469), { 0, 0, height + 24 },
                        { { 4, 2, height + 24 }, { 28, 28, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28473), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28462), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28466), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28470), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 16, 18, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28474), { 0, 0, height + 24 },
                        { { 16, 16, height + 24 }, { 16, 16, 1 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::LeftCorner, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::TopCorner, 0, height + 30,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::RightCorner, 0, height + 30,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomCorner, 0, height + 30,
                        session.SupportColours);
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
                            session, direction, session.TrackColours.WithIndex(28510), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28494), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28507), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28491), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28509), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28493), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28508), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28492), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::LeftCorner, 0, height + 30, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::TopCorner, 0, height + 30,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::RightCorner, 0, height + 30,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomCorner, 0, height + 30,
                        session.SupportColours);
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
                            session, direction, session.TrackColours.WithIndex(28522), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28506), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28519), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28503), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28521), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28505), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28520), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28504), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::LeftCorner, 0, height + 42, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::TopCorner, 0, height + 42,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::RightCorner, 0, height + 42,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomCorner, 0, height + 42,
                        session.SupportColours);
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
                            session, direction, session.TrackColours.WithIndex(28514), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28498), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28511), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28495), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28513), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28497), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28512), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28496), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::LeftCorner, 0, height + 34, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::TopCorner, 0, height + 34,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::RightCorner, 0, height + 34,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomCorner, 0, height + 34,
                        session.SupportColours);
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
                            session, direction, session.TrackColours.WithIndex(28518), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28502), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28515), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28499), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28517), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28501), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28516), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28500), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::LeftCorner, 0, height + 36, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::TopCorner, 0, height + 36,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::RightCorner, 0, height + 36,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomCorner, 0, height + 36,
                        session.SupportColours);
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
                            session, direction, session.TrackColours.WithIndex(28520), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28504), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28521), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28505), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28519), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28503), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28522), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28506), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::LeftCorner, 0, height + 42, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::TopCorner, 0, height + 42,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::RightCorner, 0, height + 42,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomCorner, 0, height + 42,
                        session.SupportColours);
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
                            session, direction, session.TrackColours.WithIndex(28516), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28500), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28517), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28501), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28515), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28499), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28518), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28502), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::LeftCorner, 0, height + 36, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::TopCorner, 0, height + 36,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::RightCorner, 0, height + 36,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomCorner, 0, height + 36,
                        session.SupportColours);
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
                            session, direction, session.TrackColours.WithIndex(28512), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28496), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28513), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28497), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28511), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28495), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28514), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28498), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::LeftCorner, 0, height + 34, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::TopCorner, 0, height + 34,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Fork, MetalSupportPlace::RightCorner, 0, height + 34,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::ForkAlt, MetalSupportPlace::BottomCorner, 0, height + 34,
                        session.SupportColours);
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
        case TrackElemType::kFlat:
            return MiniSuspendedRCTrackFlat;
        case TrackElemType::kEndStation:
        case TrackElemType::kBeginStation:
        case TrackElemType::kMiddleStation:
            return MiniSuspendedRCTrackStation;
        case TrackElemType::kUp25:
            return MiniSuspendedRCTrack25DegUp;
        case TrackElemType::kFlatToUp25:
            return MiniSuspendedRCTrackFlatTo25DegUp;
        case TrackElemType::kUp25ToFlat:
            return MiniSuspendedRCTrack25DegUpToFlat;
        case TrackElemType::kDown25:
            return MiniSuspendedRCTrack25DegDown;
        case TrackElemType::kFlatToDown25:
            return MiniSuspendedRCTrackFlatTo25DegDown;
        case TrackElemType::kDown25ToFlat:
            return MiniSuspendedRCTrack25DegDownToFlat;
        case TrackElemType::kLeftQuarterTurn5Tiles:
            return MiniSuspendedRCTrackLeftQuarterTurn5;
        case TrackElemType::kRightQuarterTurn5Tiles:
            return MiniSuspendedRCTrackRightQuarterTurn5;
        case TrackElemType::kSBendLeft:
            return MiniSuspendedRCTrackSBendLeft;
        case TrackElemType::kSBendRight:
            return MiniSuspendedRCTrackSBendRight;
        case TrackElemType::kLeftQuarterTurn3Tiles:
            return MiniSuspendedRCTrackLeftQuarterTurn3;
        case TrackElemType::kRightQuarterTurn3Tiles:
            return MiniSuspendedRCTrackRightQuarterTurn3;
        case TrackElemType::kLeftEighthToDiag:
            return MiniSuspendedRCTrackLeftEighthToDiag;
        case TrackElemType::kRightEighthToDiag:
            return MiniSuspendedRCTrackRightEighthToDiag;
        case TrackElemType::kLeftEighthToOrthogonal:
            return MiniSuspendedRCTrackLeftEighthToOrthogonal;
        case TrackElemType::kRightEighthToOrthogonal:
            return MiniSuspendedRCTrackRightEighthToOrthogonal;
        case TrackElemType::kDiagFlat:
            return MiniSuspendedRCTrackDiagFlat;
        case TrackElemType::kDiagUp25:
            return MiniSuspendedRCTrackDiag25DegUp;
        case TrackElemType::kDiagFlatToUp25:
            return MiniSuspendedRCTrackDiagFlatTo25DegUp;
        case TrackElemType::kDiagUp25ToFlat:
            return MiniSuspendedRCTrackDiag25DegUpToFlat;
        case TrackElemType::kDiagDown25:
            return MiniSuspendedRCTrackDiag25DegDown;
        case TrackElemType::kDiagFlatToDown25:
            return MiniSuspendedRCTrackDiagFlatTo25DegDown;
        case TrackElemType::kDiagDown25ToFlat:
            return MiniSuspendedRCTrackDiag25DegDownToFlat;
    }
    return nullptr;
}
