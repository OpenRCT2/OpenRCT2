/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../drawing/Drawing.h"
#include "../../../entity/EntityRegistry.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

using namespace OpenRCT2;

/** rct2: 0x008636F4 */
static void SuspendedMonorailTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height + 42, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TunnelType::InvertedSquare);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008637A4, 0x008637B4, 0x008637C4 */
static void SuspendedMonorailTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteWithChildSupportColoursPlatformless(
        session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilDrawStationInverted(session, ride, direction, height, trackElement, STATION_VARIANT_TALL))
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelType::InvertedSquare);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x00863704 */
static void SuspendedMonorailTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopRightSide, 0, height + 60, session.SupportColours);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomRightSide, 0, height + 60, session.SupportColours);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomLeftSide, 0, height + 60, session.SupportColours);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopLeftSide, 0, height + 60, session.SupportColours);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelGroup::Square, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelGroup::Square, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x00863714 */
static void SuspendedMonorailTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopRightSide, 0, height + 52, session.SupportColours);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomRightSide, 0, height + 52, session.SupportColours);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomLeftSide, 0, height + 52, session.SupportColours);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopLeftSide, 0, height + 52, session.SupportColours);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelType::InvertedSquare);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelGroup::Square, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/** rct2: 0x00863724 */
static void SuspendedMonorailTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopRightSide, 0, height + 50, session.SupportColours);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomRightSide, 0, height + 50, session.SupportColours);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomLeftSide, 0, height + 50, session.SupportColours);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopLeftSide, 0, height + 50, session.SupportColours);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelType::InvertedSquare);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelType::InvertedSquareFlatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x00863734 */
static void SuspendedMonorailTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SuspendedMonorailTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x00863744 */
static void SuspendedMonorailTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SuspendedMonorailTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x00863754 */
static void SuspendedMonorailTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SuspendedMonorailTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x00863764 */
static void SuspendedMonorailTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::Centre, 0, height + 42, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::InvertedSquare);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::Centre, 0, height + 42, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelType::InvertedSquare);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelType::InvertedSquare);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x00863774 */
static void SuspendedMonorailTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    SuspendedMonorailTrackLeftQuarterTurn5(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x00863784 */
static void SuspendedMonorailTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height + 42, 0, 0);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::InvertedSquare);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height + 42, 0, 0);

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height + 42, 0, 0);

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height + 42, 0, 0);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelType::InvertedSquare);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelType::InvertedSquare);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x00863794 */
static void SuspendedMonorailTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height + 42, 0, 0);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::InvertedSquare);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height + 42, 0, 0);

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height + 42, 0, 0);

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height + 42, 0, 0);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelType::InvertedSquare);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelType::InvertedSquare);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008637D4 */
static void SuspendedMonorailTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::Centre, 0, height + 42, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::InvertedSquare);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::Centre, 0, height + 42, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelType::InvertedSquare);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelType::InvertedSquare);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008637E4 */
static void SuspendedMonorailTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    SuspendedMonorailTrackLeftQuarterTurn3(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x00863864 */
static void SuspendedMonorailTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::Centre, 0, height + 42, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::InvertedSquare);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::BottomCorner, 0, height + 42, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::LeftCorner, 0, height + 42, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::TopCorner, 0, height + 42, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::RightCorner, 0, height + 42, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x00863874 */
static void SuspendedMonorailTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::Centre, 0, height + 42, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::InvertedSquare);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::LeftCorner, 0, height + 42, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::TopCorner, 0, height + 42, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::RightCorner, 0, height + 42, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::BottomCorner, 0, height + 42, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x00863884 */
static void SuspendedMonorailTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    SuspendedMonorailTrackRightEighthToDiag(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x00863894 */
static void SuspendedMonorailTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    SuspendedMonorailTrackLeftEighthToDiag(
        session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x008637F4 */
static void SuspendedMonorailTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(BlockedSegments::kDiagStraightFlat[trackSequence], direction), 0xFFFF, 0);

    if (trackSequence == 3)
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height + 42, session.SupportColours);

    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x00863824 */
static void SuspendedMonorailTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::LeftCorner, 0, height + 55, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::TopCorner, 0, height + 55, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::RightCorner, 0, height + 55, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::BottomCorner, 0, height + 57, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x00863804 */
static void SuspendedMonorailTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::LeftCorner, 0, height + 48, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::TopCorner, 0, height + 48, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::RightCorner, 0, height + 48, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::BottomCorner, 0, height + 50, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x00863814 */
static void SuspendedMonorailTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::LeftCorner, 0, height + 51, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::TopCorner, 0, height + 51, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::RightCorner, 0, height + 51, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::BottomCorner, 0, height + 51, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x00863854 */
static void SuspendedMonorailTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SuspendedMonorailTrackDiag25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x00863834 */
static void SuspendedMonorailTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SuspendedMonorailTrackDiag25DegUpToFlat(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x00863844 */
static void SuspendedMonorailTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    SuspendedMonorailTrackDiagFlatTo25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionSuspendedMonorail(OpenRCT2::TrackElemType trackType)
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
        default:
            return TrackPaintFunctionDummy;
    }
}
