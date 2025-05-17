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
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

namespace OpenRCT2::HybridRC
{
    static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

    static void TrackFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void TrackStation(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursStation(session, ride, trackSequence, direction, height, trackElement, supportType);

        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

        TrackPaintUtilDrawStationTunnel(session, direction, height);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void Track25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void Track60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);

        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up60Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 104);
    }

    static void TrackFlatTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::FlatToUp25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void Track25DegUpTo60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25DegToUp60Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 72);
    }

    static void Track60DegUpTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up60DegToUp25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 72);
    }

    static void Track25DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25DegToFlat);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void Track25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackFlatTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track25DegDownTo60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track60DegDownTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track25DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track90DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetVerticalTunnel(session, height + 32);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                break;
        }
    }

    static void Track90DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track60DegUpTo90DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);

        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetVerticalTunnel(session, height + 56);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                break;
        }
    }

    static void Track90DegDownTo60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);

        direction = DirectionReverse(direction);
        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetVerticalTunnel(session, height + 56);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                break;
        }
    }

    static void Track90DegUpTo60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (direction)
        {
            case 1:
                PaintUtilPushTunnelRight(session, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
                break;
            case 2:
                PaintUtilPushTunnelLeft(session, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
                break;
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 80);
    }

    static void Track60DegDownTo90DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
            case 1:
                break;
        }
    }

    static void TrackLeftQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackRightQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), (direction + 1) & 3),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        (direction + 1) & 3),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre,
                            PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        (direction + 1) & 3),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), (direction + 1) & 3),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        (direction + 1) & 3),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackRightQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthToDiag(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackRightEighthToDiag(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackLeftEighthToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackDiagFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);

        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

                break;
        }
    }

    static void TrackDiagBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackDiagBlockBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursBrake(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackDiag25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiag25DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiagFlatTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiag25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiag25DegUp(session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiagFlatTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiag25DegUpToFlat(
            session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiag25DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagFlatTo25DegUp(
            session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiag60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
        }
    }

    static void TrackDiag25DegUpTo60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackDiag60DegUpTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursChain(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackDiag60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiag60DegUp(session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiag25DegDownTo60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiag60DegUpTo25DegUp(
            session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiag60DegDownTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiag25DegUpTo60DegUp(
            session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackFlatToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void TrackFlatToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void TrackLeftBankToflat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankToflat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::FlatToUp25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackRightBankTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::FlatToUp25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void Track25DegUpToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25DegToFlat);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void Track25DegUpToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25DegToFlat);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackLeftBankTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track25DegDownToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track25DegDownToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftbank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void TrackRightbank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftbank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagFlatToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackDiagFlatToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackDiagLeftBankToflat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagFlatToRightBank(
            session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiagRightBankToflat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagFlatToLeftBank(
            session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiagLeftBankTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagRightBankTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiag25DegUpToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiag25DegUpToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiagLeftBankTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiag25DegUpToRightBank(
            session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiagRightBankTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiag25DegUpToLeftBank(
            session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiag25DegDownToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagRightBankTo25DegUp(
            session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiag25DegDownToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBankTo25DegUp(
            session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackDiagLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackDiagRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBank(session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn3Bank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackRightQuarterTurn3Bank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackBankedLeftQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), (direction + 1) & 3),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        (direction + 1) & 3),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre,
                            PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        (direction + 1) & 3),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), (direction + 1) & 3),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        (direction + 1) & 3),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackBankedRightQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthBankToDiag(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackRightEighthBankToDiag(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackLeftEighthBankToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthBankToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn3Tile25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 1:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 2:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 3:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionPrev(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackRightQuarterTurn3Tile25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 1:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 2:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 3:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionNext(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackLeftQuarterTurn3Tile25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackRightQuarterTurn3Tile25DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightQuarterTurn3Tile25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackLeftQuarterTurn3Tile25DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn5Tile25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 1:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 2:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 3:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre,
                            PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 6:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionPrev(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackRightQuarterTurn5Tile25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 1:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 2:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 3:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre,
                            PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 6:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionNext(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackLeftQuarterTurn5Tile25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackRightQuarterTurn5Tile25DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightQuarterTurn5Tile25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackLeftQuarterTurn5Tile25DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn1Tile60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (direction)
        {
            case 0:
                WoodenASupportsPaintSetup(
                    session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                break;
            case 1:
                WoodenASupportsPaintSetup(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                break;
            case 2:
                WoodenASupportsPaintSetup(
                    session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                break;
            case 3:
                WoodenASupportsPaintSetup(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                break;
        }
        TrackPaintUtilLeftQuarterTurn1TileTunnel(
            session, kTunnelGroup, direction, height, -8, TunnelSubType::SlopeStart, +56, TunnelSubType::SlopeEnd);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 104);
    }

    static void TrackRightQuarterTurn1Tile60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (direction)
        {
            case 0:
                WoodenASupportsPaintSetup(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                break;
            case 1:
                WoodenASupportsPaintSetup(
                    session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                break;
            case 2:
                WoodenASupportsPaintSetup(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                break;
            case 3:
                WoodenASupportsPaintSetup(
                    session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                break;
        }
        TrackPaintUtilRightQuarterTurn1TileTunnel(
            session, kTunnelGroup, direction, height, -8, TunnelSubType::SlopeStart, +56, TunnelSubType::SlopeEnd);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 104);
    }

    static void TrackLeftQuarterTurn1Tile60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightQuarterTurn1Tile60DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightQuarterTurn1Tile60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftQuarterTurn1Tile60DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn1Tile90DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetVerticalTunnel(session, height + 96);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 96);
                break;
            case 1:
                break;
        }
    }

    static void TrackRightQuarterTurn1Tile90DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetVerticalTunnel(session, height + 96);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 96);
                break;
            case 1:
                break;
        }
    }

    static void TrackLeftQuarterTurn1Tile90DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightQuarterTurn1Tile90DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightQuarterTurn1Tile90DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftQuarterTurn1Tile90DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void Track25DegUpToLeftBanked25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void Track25DegUpToRightBanked25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void TrackLeftBanked25DegUpTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void TrackRightBanked25DegUpTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void TrackLeftBanked25DegDownTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track25DegUpToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBanked25DegDownTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track25DegUpToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track25DegDownToLeftBanked25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBanked25DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track25DegDownToRightBanked25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBanked25DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankedFlatToLeftBanked25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::FlatToUp25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackRightBankedFlatToRightBanked25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::FlatToUp25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackLeftBanked25DegUpToLeftBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25DegToFlat);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackRightBanked25DegUpToRightBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25DegToFlat);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackLeftBankedFlatToLeftBanked25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBanked25DegUpToRightBankedFlat(
            session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankedFlatToRightBanked25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBanked25DegUpToLeftBankedFlat(
            session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBanked25DegDownToLeftBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBankedFlatToRightBanked25DegUp(
            session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBanked25DegDownToRightBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBankedFlatToLeftBanked25DegUp(
            session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track25DegUpLeftBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void Track25DegUpRightBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void Track25DegDownLeftBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track25DegUpRightBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track25DegDownRightBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track25DegUpLeftBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackFlatToLeftBanked25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::FlatToUp25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackFlatToRightBanked25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::FlatToUp25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackLeftBanked25DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25DegToFlat);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackRightBanked25DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25DegToFlat);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackFlatToLeftBanked25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBanked25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackFlatToRightBanked25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBanked25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBanked25DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBanked25DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankedQuarterTurn3Tile25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 1:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 2:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 3:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionPrev(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackRightBankedQuarterTurn3Tile25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionNext(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionNext(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackLeftBankedQuarterTurn3Tile25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackRightBankedQuarterTurn3Tile25DegUp(
            session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankedQuarterTurn3Tile25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackLeftBankedQuarterTurn3Tile25DegUp(
            session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankedQuarterTurn5Tile25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 1:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 2:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 3:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre,
                            PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 6:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionPrev(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackRightBankedQuarterTurn5Tile25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 1:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 2:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                    case 3:
                        WoodenASupportsPaintSetupRotated(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                            WoodenSupportTransitionType::Up25Deg);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre,
                            PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 6:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionNext(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackLeftBankedQuarterTurn5Tile25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackRightBankedQuarterTurn5Tile25DegUp(
            session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankedQuarterTurn5Tile25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackLeftBankedQuarterTurn5Tile25DegUp(
            session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackSBendLeft(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackSBendRight(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackLeftHalfBankedHelixUpSmall(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 7:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackRightHalfBankedHelixUpSmall(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 7:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackLeftHalfBankedHelixDownSmall(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        if (trackSequence >= 4)
        {
            trackSequence -= 4;
            direction = (direction - 1) & 3;
        }
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackRightHalfBankedHelixUpSmall(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightHalfBankedHelixDownSmall(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        if (trackSequence >= 4)
        {
            trackSequence -= 4;
            direction = (direction + 1) & 3;
        }
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackLeftHalfBankedHelixUpSmall(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackLeftHalfBankedHelixUpLarge(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight,
                            PaintSegment::top, PaintSegment::bottom, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 7:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 8:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 9:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 10:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                            PaintSegment::left, PaintSegment::right, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 11:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 12:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 13:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackRightHalfBankedHelixUpLarge(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                            PaintSegment::left, PaintSegment::right, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 7:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 8:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 9:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 10:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight,
                            PaintSegment::top, PaintSegment::bottom, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 11:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 12:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 13:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void TrackLeftHalfBankedHelixDownLarge(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        if (trackSequence >= 7)
        {
            trackSequence -= 7;
            direction = (direction - 1) & 3;
        }
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackRightHalfBankedHelixUpLarge(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightHalfBankedHelixDownLarge(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        if (trackSequence >= 7)
        {
            trackSequence -= 7;
            direction = (direction + 1) & 3;
        }
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackLeftHalfBankedHelixUpLarge(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBarrelRollUpToDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Tall);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Tall);
                        break;
                }
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackRightBarrelRollUpToDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Tall);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Tall);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackLeftBarrelRollDownToUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBarrelRollUpToDown(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBarrelRollDownToUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBarrelRollUpToDown(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftZeroGRollUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackRightZeroGRollUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::bottomRight, PaintSegment::right, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::bottomRight, PaintSegment::right, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackLeftZeroGRollDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftZeroGRollUp(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightZeroGRollDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightZeroGRollUp(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftLargeZeroGRollUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackRightLargeZeroGRollUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackLeftLargeZeroGRollDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftLargeZeroGRollUp(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightLargeZeroGRollDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightLargeZeroGRollUp(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track90DegToInvertedFlatQuarterLoopUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackInvertedFlatTo90DegQuarterLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        trackSequence = 2 - trackSequence;
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void Trackbrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void TrackOnRidePhoto(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColoursOnRidePhoto(
            session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    }

    static void TrackFlatTo60DegUpLongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq0);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq1);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq2);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq3);
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
        }
    }

    static void Track60DegUpToFlatLongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq0);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq1);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq2);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq3);
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackFlatTo60DegDownLongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track60DegUpToFlatLongBase(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void Track60DegDownToFlatLongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatTo60DegUpLongBase(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackBlockBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void Trackbooster(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void TrackpoweredLift(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void TrackLeftBankToLeftQuarterTurn3Tile25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                }
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
        }
    }

    static void TrackRightBankToRightQuarterTurn3Tile25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(
                            session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionNext(direction), height,
                    session.SupportColours);
                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
        }
    }

    static void TrackLeftQuarterTurn3Tile25DegDownToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackRightBankToRightQuarterTurn3Tile25DegUp(
            session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightQuarterTurn3Tile25DegDownToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackLeftBankToLeftQuarterTurn3Tile25DegUp(
            session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthToDiagUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::Up25Deg);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner3, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackRightEighthToDiagUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::Up25Deg);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackLeftEighthToOrthogonalUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);

                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 4:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::Up25Deg);

                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }

                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackRightEighthToOrthogonalUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NwSe, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 4:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NwSe, DirectionNext(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                if (direction == 0 || direction == 1)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackLeftEighthToDiagDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        uint8_t map[5] = { 4, 3, 1, 2, 0 };
        trackSequence = map[trackSequence];
        TrackRightEighthToOrthogonalUp25(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthToDiagDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        uint8_t map[5] = { 4, 3, 1, 2, 0 };
        trackSequence = map[trackSequence];
        TrackLeftEighthToOrthogonalUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthToOrthogonalDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackRightEighthToDiagUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthToOrthogonalDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackLeftEighthToDiagUp25(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackDiagUp25ToLeftBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackDiagUp25ToRightBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackDiagLeftBankedUp25ToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiagRightBankedUp25ToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiagDown25ToLeftBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagRightBankedUp25ToUp25(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagDown25ToRightBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBankedUp25ToUp25(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagLeftBankedDown25ToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagUp25ToRightBankedUp25(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagRightBankedDown25ToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagUp25ToLeftBankedUp25(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagLeftBankedFlatToLeftBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagRightBankedFlatToRightBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagLeftBankedUp25ToLeftBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiagRightBankedUp25ToRightBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiagLeftBankedFlatToLeftBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagRightBankedUp25ToRightBankedFlat(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagRightBankedFlatToRightBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBankedUp25ToLeftBankedFlat(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagLeftBankedDown25ToLeftBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagRightBankedFlatToRightBankedUp25(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagRightBankedDown25ToRightBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBankedFlatToLeftBankedUp25(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagUp25LeftBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiagUp25RightBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiagDown25LeftBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagUp25RightBanked(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagDown25RightBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagUp25LeftBanked(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagFlatToLeftBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagFlatToRightBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagLeftBankedUp25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiagRightBankedUp25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackDiagFlatToLeftBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagRightBankedUp25ToFlat(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagFlatToRightBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBankedUp25ToFlat(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagLeftBankedDown25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagFlatToRightBankedUp25(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagRightBankedDown25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagFlatToLeftBankedUp25(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthBankToDiagUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::Up25Deg);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner3, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackRightEighthBankToDiagUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::Up25Deg);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackLeftEighthBankToOrthogonalUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 4:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                    WoodenSupportTransitionType::Up25Deg);
                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackRightEighthBankToOrthogonalUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSprites2TrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NwSe, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 4:
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::NwSe, DirectionNext(direction), height,
                    session.SupportColours, WoodenSupportTransitionType::Up25Deg);
                if (direction == 0 || direction == 1)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackLeftEighthBankToDiagDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        uint8_t map[5] = { 4, 3, 1, 2, 0 };
        trackSequence = map[trackSequence];
        TrackRightEighthBankToOrthogonalUp25(
            session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthBankToDiagDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        uint8_t map[5] = { 4, 3, 1, 2, 0 };
        trackSequence = map[trackSequence];
        TrackLeftEighthBankToOrthogonalUp25(
            session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthBankToOrthogonalDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackRightEighthBankToDiagUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthBankToOrthogonalDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackLeftEighthBankToDiagUp25(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void Track25DegDownBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
            WoodenSupportTransitionType::Up25Deg);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelType::SquareSlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelType::SquareSlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(kSegmentsAll, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void TrackDiag25DegDownBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackPaintSpriteTrackSupportColours(session, ride, trackSequence, direction, height, trackElement, supportType);
        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                WoodenBSupportsPaintSetupRotated(
                    session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    TrackPaintFunction GetTrackPaintFunction(OpenRCT2::TrackElemType trackType)
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
            case TrackElemType::Up60:
                return Track60DegUp;
            case TrackElemType::FlatToUp25:
                return TrackFlatTo25DegUp;
            case TrackElemType::Up25ToUp60:
                return Track25DegUpTo60DegUp;
            case TrackElemType::Up60ToUp25:
                return Track60DegUpTo25DegUp;
            case TrackElemType::Up25ToFlat:
                return Track25DegUpToFlat;
            case TrackElemType::Down25:
                return Track25DegDown;
            case TrackElemType::Down60:
                return Track60DegDown;
            case TrackElemType::FlatToDown25:
                return TrackFlatTo25DegDown;
            case TrackElemType::Down25ToDown60:
                return Track25DegDownTo60DegDown;
            case TrackElemType::Down60ToDown25:
                return Track60DegDownTo25DegDown;
            case TrackElemType::Down25ToFlat:
                return Track25DegDownToFlat;
            case TrackElemType::Up90:
                return Track90DegUp;
            case TrackElemType::Down90:
                return Track90DegDown;
            case TrackElemType::Up60ToUp90:
                return Track60DegUpTo90DegUp;
            case TrackElemType::Down90ToDown60:
                return Track90DegDownTo60DegDown;
            case TrackElemType::Up90ToUp60:
                return Track90DegUpTo60DegUp;
            case TrackElemType::Down60ToDown90:
                return Track60DegDownTo90DegDown;
            case TrackElemType::LeftQuarterTurn5Tiles:
                return TrackLeftQuarterTurn5;
            case TrackElemType::RightQuarterTurn5Tiles:
                return TrackRightQuarterTurn5;
            case TrackElemType::FlatToLeftBank:
                return TrackFlatToLeftBank;
            case TrackElemType::FlatToRightBank:
                return TrackFlatToRightBank;
            case TrackElemType::LeftBankToFlat:
                return TrackLeftBankToflat;
            case TrackElemType::RightBankToFlat:
                return TrackRightBankToflat;
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
                return TrackLeftbank;
            case TrackElemType::RightBank:
                return TrackRightbank;
            case TrackElemType::LeftQuarterTurn5TilesUp25:
                return TrackLeftQuarterTurn5Tile25DegUp;
            case TrackElemType::RightQuarterTurn5TilesUp25:
                return TrackRightQuarterTurn5Tile25DegUp;
            case TrackElemType::LeftQuarterTurn5TilesDown25:
                return TrackLeftQuarterTurn5Tile25DegDown;
            case TrackElemType::RightQuarterTurn5TilesDown25:
                return TrackRightQuarterTurn5Tile25DegDown;
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
            case TrackElemType::LeftQuarterTurn1TileUp60:
                return TrackLeftQuarterTurn1Tile60DegUp;
            case TrackElemType::RightQuarterTurn1TileUp60:
                return TrackRightQuarterTurn1Tile60DegUp;
            case TrackElemType::LeftQuarterTurn1TileDown60:
                return TrackLeftQuarterTurn1Tile60DegDown;
            case TrackElemType::RightQuarterTurn1TileDown60:
                return TrackRightQuarterTurn1Tile60DegDown;
            case TrackElemType::Brakes:
                return Trackbrakes;
            case TrackElemType::Up25LeftBanked:
                return Track25DegUpLeftBanked;
            case TrackElemType::Up25RightBanked:
                return Track25DegUpRightBanked;
            case TrackElemType::OnRidePhoto:
                return TrackOnRidePhoto;
            case TrackElemType::Down25LeftBanked:
                return Track25DegDownLeftBanked;
            case TrackElemType::Down25RightBanked:
                return Track25DegDownRightBanked;
            case TrackElemType::FlatToUp60LongBase:
                return TrackFlatTo60DegUpLongBase;
            case TrackElemType::Up60ToFlatLongBase:
                return Track60DegUpToFlatLongBase;
            case TrackElemType::FlatToDown60LongBase:
                return TrackFlatTo60DegDownLongBase;
            case TrackElemType::Down60ToFlatLongBase:
                return Track60DegDownToFlatLongBase;
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
            case TrackElemType::DiagBrakes:
                return TrackDiagBrakes;
            case TrackElemType::DiagBlockBrakes:
                return TrackDiagBlockBrakes;
            case TrackElemType::DiagUp25:
                return TrackDiag25DegUp;
            case TrackElemType::DiagUp60:
                return TrackDiag60DegUp;
            case TrackElemType::DiagFlatToUp25:
                return TrackDiagFlatTo25DegUp;
            case TrackElemType::DiagUp25ToUp60:
                return TrackDiag25DegUpTo60DegUp;
            case TrackElemType::DiagUp60ToUp25:
                return TrackDiag60DegUpTo25DegUp;
            case TrackElemType::DiagUp25ToFlat:
                return TrackDiag25DegUpToFlat;
            case TrackElemType::DiagDown25:
                return TrackDiag25DegDown;
            case TrackElemType::DiagDown60:
                return TrackDiag60DegDown;
            case TrackElemType::DiagFlatToDown25:
                return TrackDiagFlatTo25DegDown;
            case TrackElemType::DiagDown25ToDown60:
                return TrackDiag25DegDownTo60DegDown;
            case TrackElemType::DiagDown60ToDown25:
                return TrackDiag60DegDownTo25DegDown;
            case TrackElemType::DiagDown25ToFlat:
                return TrackDiag25DegDownToFlat;
            case TrackElemType::DiagFlatToLeftBank:
                return TrackDiagFlatToLeftBank;
            case TrackElemType::DiagFlatToRightBank:
                return TrackDiagFlatToRightBank;
            case TrackElemType::DiagLeftBankToFlat:
                return TrackDiagLeftBankToflat;
            case TrackElemType::DiagRightBankToFlat:
                return TrackDiagRightBankToflat;
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
            case TrackElemType::BlockBrakes:
                return TrackBlockBrakes;
            case TrackElemType::LeftBankedQuarterTurn3TileUp25:
                return TrackLeftBankedQuarterTurn3Tile25DegUp;
            case TrackElemType::RightBankedQuarterTurn3TileUp25:
                return TrackRightBankedQuarterTurn3Tile25DegUp;
            case TrackElemType::LeftBankedQuarterTurn3TileDown25:
                return TrackLeftBankedQuarterTurn3Tile25DegDown;
            case TrackElemType::RightBankedQuarterTurn3TileDown25:
                return TrackRightBankedQuarterTurn3Tile25DegDown;
            case TrackElemType::LeftBankedQuarterTurn5TileUp25:
                return TrackLeftBankedQuarterTurn5Tile25DegUp;
            case TrackElemType::RightBankedQuarterTurn5TileUp25:
                return TrackRightBankedQuarterTurn5Tile25DegUp;
            case TrackElemType::LeftBankedQuarterTurn5TileDown25:
                return TrackLeftBankedQuarterTurn5Tile25DegDown;
            case TrackElemType::RightBankedQuarterTurn5TileDown25:
                return TrackRightBankedQuarterTurn5Tile25DegDown;
            case TrackElemType::Up25ToLeftBankedUp25:
                return Track25DegUpToLeftBanked25DegUp;
            case TrackElemType::Up25ToRightBankedUp25:
                return Track25DegUpToRightBanked25DegUp;
            case TrackElemType::LeftBankedUp25ToUp25:
                return TrackLeftBanked25DegUpTo25DegUp;
            case TrackElemType::RightBankedUp25ToUp25:
                return TrackRightBanked25DegUpTo25DegUp;
            case TrackElemType::Down25ToLeftBankedDown25:
                return Track25DegDownToLeftBanked25DegDown;
            case TrackElemType::Down25ToRightBankedDown25:
                return Track25DegDownToRightBanked25DegDown;
            case TrackElemType::LeftBankedDown25ToDown25:
                return TrackLeftBanked25DegDownTo25DegDown;
            case TrackElemType::RightBankedDown25ToDown25:
                return TrackRightBanked25DegDownTo25DegDown;
            case TrackElemType::LeftBankedFlatToLeftBankedUp25:
                return TrackLeftBankedFlatToLeftBanked25DegUp;
            case TrackElemType::RightBankedFlatToRightBankedUp25:
                return TrackRightBankedFlatToRightBanked25DegUp;
            case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
                return TrackLeftBanked25DegUpToLeftBankedFlat;
            case TrackElemType::RightBankedUp25ToRightBankedFlat:
                return TrackRightBanked25DegUpToRightBankedFlat;
            case TrackElemType::LeftBankedFlatToLeftBankedDown25:
                return TrackLeftBankedFlatToLeftBanked25DegDown;
            case TrackElemType::RightBankedFlatToRightBankedDown25:
                return TrackRightBankedFlatToRightBanked25DegDown;
            case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
                return TrackLeftBanked25DegDownToLeftBankedFlat;
            case TrackElemType::RightBankedDown25ToRightBankedFlat:
                return TrackRightBanked25DegDownToRightBankedFlat;
            case TrackElemType::FlatToLeftBankedUp25:
                return TrackFlatToLeftBanked25DegUp;
            case TrackElemType::FlatToRightBankedUp25:
                return TrackFlatToRightBanked25DegUp;
            case TrackElemType::LeftBankedUp25ToFlat:
                return TrackLeftBanked25DegUpToFlat;
            case TrackElemType::RightBankedUp25ToFlat:
                return TrackRightBanked25DegUpToFlat;
            case TrackElemType::FlatToLeftBankedDown25:
                return TrackFlatToLeftBanked25DegDown;
            case TrackElemType::FlatToRightBankedDown25:
                return TrackFlatToRightBanked25DegDown;
            case TrackElemType::LeftBankedDown25ToFlat:
                return TrackLeftBanked25DegDownToFlat;
            case TrackElemType::RightBankedDown25ToFlat:
                return TrackRightBanked25DegDownToFlat;
            case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
                return TrackLeftBankToLeftQuarterTurn3Tile25DegUp;
            case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
                return TrackRightBankToRightQuarterTurn3Tile25DegUp;
            case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
                return TrackLeftQuarterTurn3Tile25DegDownToLeftBank;
            case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
                return TrackRightQuarterTurn3Tile25DegDownToRightBank;
            case TrackElemType::LeftQuarterTurn1TileUp90:
                return TrackLeftQuarterTurn1Tile90DegUp;
            case TrackElemType::RightQuarterTurn1TileUp90:
                return TrackRightQuarterTurn1Tile90DegUp;
            case TrackElemType::LeftQuarterTurn1TileDown90:
                return TrackLeftQuarterTurn1Tile90DegDown;
            case TrackElemType::RightQuarterTurn1TileDown90:
                return TrackRightQuarterTurn1Tile90DegDown;
            case TrackElemType::LeftBarrelRollUpToDown:
                return TrackLeftBarrelRollUpToDown;
            case TrackElemType::RightBarrelRollUpToDown:
                return TrackRightBarrelRollUpToDown;
            case TrackElemType::LeftBarrelRollDownToUp:
                return TrackLeftBarrelRollDownToUp;
            case TrackElemType::RightBarrelRollDownToUp:
                return TrackRightBarrelRollDownToUp;
            case TrackElemType::LeftZeroGRollUp:
                return TrackLeftZeroGRollUp;
            case TrackElemType::RightZeroGRollUp:
                return TrackRightZeroGRollUp;
            case TrackElemType::LeftZeroGRollDown:
                return TrackLeftZeroGRollDown;
            case TrackElemType::RightZeroGRollDown:
                return TrackRightZeroGRollDown;
            case TrackElemType::LeftLargeZeroGRollUp:
                return TrackLeftLargeZeroGRollUp;
            case TrackElemType::RightLargeZeroGRollUp:
                return TrackRightLargeZeroGRollUp;
            case TrackElemType::LeftLargeZeroGRollDown:
                return TrackLeftLargeZeroGRollDown;
            case TrackElemType::RightLargeZeroGRollDown:
                return TrackRightLargeZeroGRollDown;
            case TrackElemType::Up90ToInvertedFlatQuarterLoop:
                return Track90DegToInvertedFlatQuarterLoopUp;
            case TrackElemType::InvertedFlatToDown90QuarterLoop:
                return TrackInvertedFlatTo90DegQuarterLoopDown;

            case TrackElemType::LeftEighthToDiagUp25:
                return TrackLeftEighthToDiagUp25;
            case TrackElemType::RightEighthToDiagUp25:
                return TrackRightEighthToDiagUp25;
            case TrackElemType::LeftEighthToDiagDown25:
                return TrackLeftEighthToDiagDown25;
            case TrackElemType::RightEighthToDiagDown25:
                return TrackRightEighthToDiagDown25;
            case TrackElemType::LeftEighthToOrthogonalUp25:
                return TrackLeftEighthToOrthogonalUp25;
            case TrackElemType::RightEighthToOrthogonalUp25:
                return TrackRightEighthToOrthogonalUp25;
            case TrackElemType::LeftEighthToOrthogonalDown25:
                return TrackLeftEighthToOrthogonalDown25;
            case TrackElemType::RightEighthToOrthogonalDown25:
                return TrackRightEighthToOrthogonalDown25;
            case TrackElemType::DiagUp25ToLeftBankedUp25:
                return TrackDiagUp25ToLeftBankedUp25;
            case TrackElemType::DiagUp25ToRightBankedUp25:
                return TrackDiagUp25ToRightBankedUp25;
            case TrackElemType::DiagLeftBankedUp25ToUp25:
                return TrackDiagLeftBankedUp25ToUp25;
            case TrackElemType::DiagRightBankedUp25ToUp25:
                return TrackDiagRightBankedUp25ToUp25;
            case TrackElemType::DiagDown25ToLeftBankedDown25:
                return TrackDiagDown25ToLeftBankedDown25;
            case TrackElemType::DiagDown25ToRightBankedDown25:
                return TrackDiagDown25ToRightBankedDown25;
            case TrackElemType::DiagLeftBankedDown25ToDown25:
                return TrackDiagLeftBankedDown25ToDown25;
            case TrackElemType::DiagRightBankedDown25ToDown25:
                return TrackDiagRightBankedDown25ToDown25;
            case TrackElemType::DiagLeftBankedFlatToLeftBankedUp25:
                return TrackDiagLeftBankedFlatToLeftBankedUp25;
            case TrackElemType::DiagRightBankedFlatToRightBankedUp25:
                return TrackDiagRightBankedFlatToRightBankedUp25;
            case TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat:
                return TrackDiagLeftBankedUp25ToLeftBankedFlat;
            case TrackElemType::DiagRightBankedUp25ToRightBankedFlat:
                return TrackDiagRightBankedUp25ToRightBankedFlat;
            case TrackElemType::DiagLeftBankedFlatToLeftBankedDown25:
                return TrackDiagLeftBankedFlatToLeftBankedDown25;
            case TrackElemType::DiagRightBankedFlatToRightBankedDown25:
                return TrackDiagRightBankedFlatToRightBankedDown25;
            case TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat:
                return TrackDiagLeftBankedDown25ToLeftBankedFlat;
            case TrackElemType::DiagRightBankedDown25ToRightBankedFlat:
                return TrackDiagRightBankedDown25ToRightBankedFlat;
            case TrackElemType::DiagUp25LeftBanked:
                return TrackDiagUp25LeftBanked;
            case TrackElemType::DiagUp25RightBanked:
                return TrackDiagUp25RightBanked;
            case TrackElemType::DiagDown25LeftBanked:
                return TrackDiagDown25LeftBanked;
            case TrackElemType::DiagDown25RightBanked:
                return TrackDiagDown25RightBanked;
            case TrackElemType::DiagFlatToLeftBankedUp25:
                return TrackDiagFlatToLeftBankedUp25;
            case TrackElemType::DiagFlatToRightBankedUp25:
                return TrackDiagFlatToRightBankedUp25;
            case TrackElemType::DiagLeftBankedUp25ToFlat:
                return TrackDiagLeftBankedUp25ToFlat;
            case TrackElemType::DiagRightBankedUp25ToFlat:
                return TrackDiagRightBankedUp25ToFlat;
            case TrackElemType::DiagFlatToLeftBankedDown25:
                return TrackDiagFlatToLeftBankedDown25;
            case TrackElemType::DiagFlatToRightBankedDown25:
                return TrackDiagFlatToRightBankedDown25;
            case TrackElemType::DiagLeftBankedDown25ToFlat:
                return TrackDiagLeftBankedDown25ToFlat;
            case TrackElemType::DiagRightBankedDown25ToFlat:
                return TrackDiagRightBankedDown25ToFlat;
            case TrackElemType::LeftEighthBankToDiagUp25:
                return TrackLeftEighthBankToDiagUp25;
            case TrackElemType::RightEighthBankToDiagUp25:
                return TrackRightEighthBankToDiagUp25;
            case TrackElemType::LeftEighthBankToDiagDown25:
                return TrackLeftEighthBankToDiagDown25;
            case TrackElemType::RightEighthBankToDiagDown25:
                return TrackRightEighthBankToDiagDown25;
            case TrackElemType::LeftEighthBankToOrthogonalUp25:
                return TrackLeftEighthBankToOrthogonalUp25;
            case TrackElemType::RightEighthBankToOrthogonalUp25:
                return TrackRightEighthBankToOrthogonalUp25;
            case TrackElemType::LeftEighthBankToOrthogonalDown25:
                return TrackLeftEighthBankToOrthogonalDown25;
            case TrackElemType::RightEighthBankToOrthogonalDown25:
                return TrackRightEighthBankToOrthogonalDown25;

            case TrackElemType::PoweredLift:
                return TrackpoweredLift;
            case TrackElemType::Booster:
                return Trackbooster;
            case TrackElemType::Down25Brakes:
                return Track25DegDownBrakes;
            case TrackElemType::DiagDown25Brakes:
                return TrackDiag25DegDownBrakes;

            default:
                return TrackPaintFunctionDummy;
        }
    }
} // namespace OpenRCT2::HybridRC
