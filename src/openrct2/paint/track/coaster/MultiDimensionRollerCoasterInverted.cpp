/*****************************************************************************
 * copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../ride/TrackPaint.h"
#include "../../track/Segment.h"

namespace OpenRCT2::MultiDimensionRCInverted
{
    static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

    static constexpr ImageIndex MULTI_DIM_INVERTED_BLOCK_BRAKE_SW_NE_OPEN = 26551;
    static constexpr ImageIndex MULTI_DIM_INVERTED_BLOCK_BRAKE_NW_SE_OPEN = 26552;
    static constexpr ImageIndex MULTI_DIM_INVERTED_BLOCK_BRAKE_SW_NE_CLOSED = 26553;
    static constexpr ImageIndex MULTI_DIM_INVERTED_BLOCK_BRAKE_NW_SE_CLOSED = 26554;

    static constexpr const ImageIndex kMultidimDiagBrakeImages[kNumOrthogonalDirections] = {
        SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES,
        SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES + 1,
        SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES,
        SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES + 1,
    };

    static constexpr const ImageIndex kMultidimDiagBlockBrakeImages[2][kNumOrthogonalDirections] = {
        {
            SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES + 2,
            SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES + 4,
            SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES + 2,
            SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES + 4,
        },
        {
            SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES + 2,
            SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES + 3,
            SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES + 2,
            SPR_TRACKS_MULTIDIM_INVERTED_DIAG_BRAKES + 3,
        },
    };

    static void trackPaintFlat(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26227), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 1 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26228), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 1 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);
        }

        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void trackPaintUp25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26253), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26254), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26255), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26256), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 54, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 54, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 54, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 54, session.SupportColours);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void trackPaintUp60(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26269), { 0, 0, height + 24 },
                    { { 0, 6, height + 88 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26270), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 81 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26271), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 81 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26272), { 0, 0, height + 24 },
                    { { 0, 6, height + 88 }, { 32, 20, 3 } });
                break;
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 104);
    }

    static void trackPaintFlatToUp25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26245), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26246), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26247), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26248), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 46, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 46, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 46, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 46, session.SupportColours);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void trackPaintUp25ToUp60(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26257), { 0, 0, height + 24 },
                    { { 0, 6, height + 56 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26261), { 0, 0, height + 24 },
                    { { 0, 10, height + 6 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26258), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 49 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26262), { 0, 0, height + 24 },
                    { { 0, 10, height + 6 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26259), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 49 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26260), { 0, 0, height + 24 },
                    { { 0, 6, height + 56 }, { 32, 20, 3 } });
                break;
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 72);
    }

    static void trackPaintUp60ToUp25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26263), { 0, 0, height + 24 },
                    { { 0, 6, height + 56 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26267), { 0, 0, height + 24 },
                    { { 0, 10, height + 6 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26264), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 49 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26268), { 0, 0, height + 24 },
                    { { 0, 10, height + 6 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26265), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 49 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26266), { 0, 0, height + 24 },
                    { { 0, 6, height + 56 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 68, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 68, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 68, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 68, session.SupportColours);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetGeneralSupportHeight(session, height + 72);
    }

    static void trackPaintUp25ToFlat(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26249), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26250), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26251), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26252), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 44, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 44, session.SupportColours);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void trackPaintDown25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintDown60(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintUp60(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintFlatToDown25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintUp25ToFlat(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintDown25ToDown60(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintUp60ToUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintDown60ToDown25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintUp25ToUp60(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintDown25ToFlat(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintFlatToUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintLeftQuarterTurn5Tiles(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26310), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26315), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26320), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26305), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                            PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26309), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26314), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26319), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26304), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26308), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26313), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26318), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26303), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26307), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26312), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26317), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26302), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 32, 3 } });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26306), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26311), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26316), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26301), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                            PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintRightQuarterTurn5Tiles(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintLeftQuarterTurn5Tiles(
            session, ride, kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence], DirectionPrev(direction), height,
            trackElement, supportType);
    }

    static void trackPaintFlatToLeftBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26273), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26274), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26275), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26276), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
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
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);
        }

        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void trackPaintFlatToRightBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26277), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26278), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26279), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26280), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(
                    PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                    PaintSegment::bottomLeft),
                direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);
        }

        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void trackPaintLeftBankToFlat(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26279), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26280), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26277), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26278), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
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
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);
        }

        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void trackPaintRightBankToFlat(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26275), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26276), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26273), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26274), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(
                    PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                    PaintSegment::bottomLeft),
                direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);
        }

        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void trackPaintBankedLeftQuarterTurn5Tiles(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26354), { 0, 0, height + 24 },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26359), { 0, 0, height + 24 },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26364), { 0, 0, height + 24 },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26349), { 0, 0, height + 24 },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                            PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26353), { 0, 0, height + 24 }, { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26358), { 0, 0, height + 24 }, { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26363), { 0, 0, height + 24 },
                            { { 0, 16, height + 24 }, { 32, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26348), { 0, 0, height + 24 },
                            { { 0, 16, height + 24 }, { 32, 16, 3 } });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26352), { 0, 0, height + 24 },
                            { { 0, 16, height + 24 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26357), { 0, 0, height + 24 },
                            { { 16, 16, height + 24 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26362), { 0, 0, height + 24 },
                            { { 16, 0, height + 24 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26347), { 0, 0, height + 24 }, { 16, 16, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26351), { 0, 0, height + 24 },
                            { { 16, 0, height + 24 }, { 16, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26356), { 0, 0, height + 24 }, { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26361), { 0, 0, height + 24 }, { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26346), { 0, 0, height + 24 },
                            { { 16, 0, height + 24 }, { 16, 32, 3 } });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26350), { 0, 0, height + 24 },
                            { { 6, 0, height + 24 }, { 20, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26355), { 0, 0, height + 24 },
                            { { 6, 0, height + 24 }, { 20, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26360), { 0, 0, height + 24 },
                            { { 6, 0, height + 24 }, { 20, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26345), { 0, 0, height + 24 },
                            { { 6, 0, height + 24 }, { 20, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                            PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintBankedRightQuarterTurn5Tiles(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintBankedLeftQuarterTurn5Tiles(
            session, ride, kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence], DirectionPrev(direction), height,
            trackElement, supportType);
    }

    static void trackPaintLeftBankToUp25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26281), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26282), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26283), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26284), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
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
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 44, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 44, session.SupportColours);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void trackPaintRightBankToUp25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26285), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26286), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26287), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26288), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(
                    PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                    PaintSegment::bottomLeft),
                direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 44, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 44, session.SupportColours);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void trackPaintUp25ToLeftBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26289), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26290), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26291), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26292), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
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
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 44, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 44, session.SupportColours);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void trackPaintUp25ToRightBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26293), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26294), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26295), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26296), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(
                    PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                    PaintSegment::bottomLeft),
                direction),
            0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 44, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 44, session.SupportColours);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void trackPaintLeftBankToDown25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintUp25ToRightBank(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintRightBankToDown25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintUp25ToLeftBank(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintDown25ToLeftBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintRightBankToUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintDown25ToRightBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintLeftBankToUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintLeftBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26297), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26298), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26299), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26300), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 3 } });
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
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);
        }

        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void trackPaintRightBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintLeftBank(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintSBendLeft(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26229), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26233), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26232), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26236), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26230), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26234), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26231), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26235), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
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
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26231), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26235), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26230), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26234), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
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
                switch (direction)
                {
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26232), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26236), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26229), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26233), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintSBendRight(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26237), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26241), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26240), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26244), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height + 36, 0, 0);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26238), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26242), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26239), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26243), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
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
                DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height + 36, 0, 0);

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26239), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26243), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26238), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26242), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 26, 3 } });
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
                DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height + 36, 0, 0);

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26240), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26244), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26237), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26241), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height + 36, 0, 0);

                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintLeftQuarterTurn3Tiles(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26326), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26329), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26332), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26323), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                            PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26325), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26328), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26331), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26322), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26324), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26327), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26330), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26321), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                            PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintRightQuarterTurn3Tiles(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintLeftQuarterTurn3Tiles(
            session, ride, kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence], DirectionPrev(direction), height,
            trackElement, supportType);
    }

    static void trackPaintLeftBankedQuarterTurn3Tiles(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26338), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26341), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26344), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26335), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                            PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26337), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26340), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26343), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26334), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26336), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26339), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26342), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26333), { 0, 0, height + 24 },
                            { { 6, 0, height + 22 }, { 20, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                            PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintRightBankedQuarterTurn3Tiles(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintLeftBankedQuarterTurn3Tiles(
            session, ride, kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence], DirectionPrev(direction), height,
            trackElement, supportType);
    }

    static void trackPaintBrakes(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26549), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26550), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);
        }

        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void trackPaintOnRidePhoto(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        TrackPaintUtilOnridePhotoPlatformPaintBase(session, height);
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height, session.SupportColours);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height, session.SupportColours);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26227), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::topRightSide, 0, height, session.SupportColours);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height, session.SupportColours);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26228), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height, session.SupportColours);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height, session.SupportColours);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26227), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::topRightSide, 0, height, session.SupportColours);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height, session.SupportColours);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26228), { 0, 0, height + 24 },
                    { { 0, 6, height + 24 }, { 32, 20, 3 } });
                break;
        }
        TrackPaintUtilOnridePhotoPaint2(session, direction, trackElement, height, kGeneralSupportHeightOnRidePhotoInverted);
    }

    static void trackPaintUp90(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26533), { 0, 0, height + 24 },
                            { { 0, 6, height + 56 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26534), { 0, 0, height + 24 },
                            { { 0, 4, height + 6 }, { 32, 2, 31 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26535), { 0, 0, height + 24 },
                            { { 0, 4, height + 6 }, { 32, 2, 31 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26536), { 0, 0, height + 24 },
                            { { 0, 6, height + 56 }, { 32, 20, 3 } });
                        break;
                }
                PaintUtilSetVerticalTunnel(session, height + 32);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                break;
        }
    }

    static void trackPaintDown90(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintUp90(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintUp60ToUp90(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26525), { 0, 0, height + 24 },
                            { { 0, 6, height + 80 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26526), { 0, 0, height + 24 },
                            { { 0, 4, height + 6 }, { 32, 2, 55 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26527), { 0, 0, height + 24 },
                            { { 0, 4, height + 6 }, { 32, 2, 55 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26528), { 0, 0, height + 24 },
                            { { 0, 6, height + 80 }, { 32, 20, 3 } });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(
                        session, direction, height - 8, TunnelGroup::Inverted, TunnelSubType::SlopeStart);
                }
                PaintUtilSetVerticalTunnel(session, height + 56);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                break;
        }
    }

    static void trackPaintDown90ToDown60(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintUp60ToUp90(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
    }

    static void trackPaintUp90ToUp60(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26529), { 0, 0, height + 24 },
                    { { 0, 6, height + 80 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26530), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 55 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26531), { 0, 0, height + 24 },
                    { { 0, 4, height + 6 }, { 32, 2, 55 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26532), { 0, 0, height + 24 },
                    { { 0, 6, height + 80 }, { 32, 20, 3 } });
                break;
        }
        switch (direction)
        {
            case 1:
                PaintUtilPushTunnelRight(session, height + 48, TunnelGroup::Inverted, TunnelSubType::SlopeEnd);
                break;
            case 2:
                PaintUtilPushTunnelLeft(session, height + 48, TunnelGroup::Inverted, TunnelSubType::SlopeEnd);
                break;
        }
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 96);
    }

    static void trackPaintDown60ToDown90(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26531), { 0, 0, height + 24 },
                            { { 0, 4, height + 6 }, { 32, 2, 55 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26532), { 0, 0, height + 24 },
                            { { 0, 6, height + 80 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26529), { 0, 0, height + 24 },
                            { { 0, 6, height + 80 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26530), { 0, 0, height + 24 },
                            { { 0, 4, height + 6 }, { 32, 2, 55 } });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 48, TunnelGroup::Inverted, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 96);
                break;
            case 1:
                break;
        }
    }

    static void trackPaintLeftEighthToDiag(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26405), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26409), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26413), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26417), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26406), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26410), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 34, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26414), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26418), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26407), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26411), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26415), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26419), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26408), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26412), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 18, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26416), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26420), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintRightEighthToDiag(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26389), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26393), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26397), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26401), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26390), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26394), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26398), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 34, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26402), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26391), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26395), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26399), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26403), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26392), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26396), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26400), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 18, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26404), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintLeftEighthToOrthogonal(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintRightEighthToDiag(
            session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionReverse(direction), height, trackElement,
            supportType);
    }

    static void trackPaintRightEighthToOrthogonal(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintLeftEighthToDiag(
            session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionPrev(direction), height, trackElement,
            supportType);
    }

    static void trackPaintLeftEighthBankToDiag(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26437), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26441), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26445), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26449), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26438), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26442), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 34, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26446), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26450), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26439), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26443), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26447), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26451), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26440), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26444), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 18, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26448), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26452), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintRightEighthBankToDiag(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26421), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26425), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26429), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26433), { 0, 0, height + 24 },
                            { { 0, 6, height + 22 }, { 32, 20, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26422), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26426), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 32, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26430), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 34, 16, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26434), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 32, 16, 3 } });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26423), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26427), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26431), { 0, 0, height + 24 },
                            { { 4, 4, height + 22 }, { 28, 28, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26435), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26424), { 0, 0, height + 24 },
                            { { 16, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26428), { 0, 0, height + 24 },
                            { { 0, 0, height + 22 }, { 16, 16, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26432), { 0, 0, height + 24 },
                            { { 0, 16, height + 22 }, { 16, 18, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26436), { 0, 0, height + 24 },
                            { { 16, 16, height + 22 }, { 16, 16, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintLeftEighthBankToOrthogonal(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintRightEighthBankToDiag(
            session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionReverse(direction), height, trackElement,
            supportType);
    }

    static void trackPaintRightEighthBankToOrthogonal(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintLeftEighthBankToDiag(
            session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionPrev(direction), height, trackElement,
            supportType);
    }

    static void trackPaintDiagFlat(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
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
                                session, direction, session.TrackColours.WithIndex(26484), { -16, -16, height + 24 },
                                { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26456), { -16, -16, height + 24 },
                                { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }
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
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26481), { -16, -16, height + 24 },
                                { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26453), { -16, -16, height + 24 },
                                { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26483), { -16, -16, height + 24 },
                                { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26455), { -16, -16, height + 24 },
                                { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
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
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26482), { -16, -16, height + 24 },
                                { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26454), { -16, -16, height + 24 },
                                { { -16, -16, height + 22 }, { 32, 32, 3 } });
                            break;
                    }
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintDiagUp25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
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
                                session, direction, session.TrackColours.WithIndex(26496), { -16, -16, height + 24 },
                                { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26468), { -16, -16, height + 24 },
                                { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26493), { -16, -16, height + 24 },
                                { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26465), { -16, -16, height + 24 },
                                { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26495), { -16, -16, height + 24 },
                                { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26467), { -16, -16, height + 24 },
                                { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
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
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26494), { -16, -16, height + 24 },
                                { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26466), { -16, -16, height + 24 },
                                { { -16, -16, height + 40 }, { 32, 32, 3 } });
                            break;
                    }
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 48, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 48, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 48, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 48,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void trackPaintDiagUp60(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26480), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26477), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26479), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26478), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 32, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 36, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 32, height + 36,
                            session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 36, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
        }
    }

    static void trackPaintDiagFlatToUp25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
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
                                session, direction, session.TrackColours.WithIndex(26488), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26460), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26485), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26457), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26487), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26459), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
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
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26486), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26458), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 40,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void trackPaintDiagUp25ToUp60(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26472), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26469), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26471), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26470), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 16, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 16, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 16, height + 36,
                            session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 16, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void trackPaintDiagUp60ToUp25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26476), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26473), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26475), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26474), { -16, -16, height + 24 },
                            { { 0, 0, height + 56 }, { 16, 16, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 21, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 21, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 21, height + 36,
                            session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 21, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void trackPaintDiagUp25ToFlat(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
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
                                session, direction, session.TrackColours.WithIndex(26492), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26464), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26489), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26461), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26491), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26463), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
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
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26490), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours.WithIndex(26462), { -16, -16, height + 24 },
                                { { -16, -16, height + 32 }, { 32, 32, 3 } });
                            break;
                    }
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 42,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void trackPaintDiagDown25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26466), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26467), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26465), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26468), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 48, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 48, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 48, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 48,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void trackPaintDiagDown60(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26478), { -16, -16, height + 24 },
                            { { -16, -16, height + 24 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26479), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26477), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26480), { -16, -16, height + 24 },
                            { { -16, -16, height + 88 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 24, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 28, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 24, height + 36,
                            session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 28, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
        }
    }

    static void trackPaintDiagFlatToDown25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26462), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26463), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26461), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26464), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 42,
                            session.SupportColours);
                        break;
                }
                break;
        }

        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void trackPaintDiagDown25ToDown60(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26474), { -16, -16, height + 24 },
                            { { 0, 0, height + 56 }, { 16, 16, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26475), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26473), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26476), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 17, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 17, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 17, height + 36,
                            session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 17, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void trackPaintDiagDown60ToDown25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26470), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26471), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26469), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26472), { -16, -16, height + 24 },
                            { { -16, -16, height + 56 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 8, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 8, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 8, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 8, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void trackPaintDiagDown25ToFlat(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26458), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26459), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26457), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26460), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 40,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void trackPaintDiagFlatToLeftBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26504), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26501), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26503), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26502), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintDiagFlatToRightBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26508), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26505), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26507), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26506), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintDiagLeftBankToFlat(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26506), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26507), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26505), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26508), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintDiagRightBankToFlat(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26502), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26503), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26501), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26504), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintDiagLeftBankToUp25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26520), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26517), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26519), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26518), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 40,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void trackPaintDiagRightBankToUp25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26524), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26521), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26523), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26522), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 40,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void trackPaintDiagUp25ToLeftBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26512), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26509), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26511), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26510), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 42,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void trackPaintDiagUp25ToRightBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26516), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26513), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26515), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26514), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 42,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void trackPaintDiagLeftBankToDown25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26514), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26515), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26513), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26516), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 42,
                            session.SupportColours);
                        break;
                }
                break;
        }

        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void trackPaintDiagRightBankToDown25(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26510), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26511), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26509), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26512), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 42, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 42,
                            session.SupportColours);
                        break;
                }
                break;
        }

        PaintUtilSetGeneralSupportHeight(session, height + 56);
    }

    static void trackPaintDiagDown25ToLeftBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26522), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26523), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26521), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26524), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 40,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void trackPaintDiagDown25ToRightBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26518), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26519), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26517), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
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
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26520), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }

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
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 40, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 40,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void trackPaintDiagLeftBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26500), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26497), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26499), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26498), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintDiagRightBank(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26498), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26499), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26497), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26500), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                            PaintSegment::bottomLeft),
                        direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 36, session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 36,
                            session.SupportColours);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintLeftFlyerTwistDown(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26365), { 0, 6, height + 24 }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26371), { 0, 6, height + 24 }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26370), { 0, 6, height + 24 },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26376), { 0, 6, height + 24 }, { 32, 20, 3 });
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
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26366), { 0, 6, height + 24 }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26372), { 0, 6, height + 24 }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26369), { 0, 6, height + 24 },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26375), { 0, 6, height + 24 }, { 32, 20, 3 });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26367), { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26373), { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26368), { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26374), { 0, 6, height }, { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height, session.SupportColours);
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
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
        }
    }

    static void trackPaintRightFlyerTwistDown(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26377), { 0, 6, height + 24 }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26383), { 0, 6, height + 24 }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26382), { 0, 6, height + 24 }, { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26388), { 0, 6, height + 24 }, { 32, 20, 3 });
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
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26378), { 0, 6, height + 24 }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26384), { 0, 6, height + 24 }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26381), { 0, 6, height + 24 }, { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26387), { 0, 6, height + 24 }, { 32, 20, 3 });
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
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26379), { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26385), { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26380), { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26386), { 0, 6, height }, { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 0, height, session.SupportColours);
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
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
        }
    }

    static void trackPaintBlockBrakes(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        const bool isClosed = trackElement.IsBrakeClosed();
        static constexpr uint32_t imageIds[kNumOrthogonalDirections][2] = {
            { MULTI_DIM_INVERTED_BLOCK_BRAKE_SW_NE_OPEN, MULTI_DIM_INVERTED_BLOCK_BRAKE_SW_NE_CLOSED },
            { MULTI_DIM_INVERTED_BLOCK_BRAKE_NW_SE_OPEN, MULTI_DIM_INVERTED_BLOCK_BRAKE_NW_SE_CLOSED },
            { MULTI_DIM_INVERTED_BLOCK_BRAKE_SW_NE_OPEN, MULTI_DIM_INVERTED_BLOCK_BRAKE_SW_NE_CLOSED },
            { MULTI_DIM_INVERTED_BLOCK_BRAKE_NW_SE_OPEN, MULTI_DIM_INVERTED_BLOCK_BRAKE_NW_SE_CLOSED },
        };

        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(imageIds[direction][isClosed]), { 0, 0, height + 24 },
            { { 0, 6, height + 24 }, { 32, 20, 3 } });

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 36, session.SupportColours);
        }

        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
    }

    static void trackPaintMultiDimInvertedFlatToDown90QuarterLoop(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16208), { 0, 0, height },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16211), { 0, 0, height },
                            { { 24, 6, height + 8 }, { 2, 20, 31 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16214), { 0, 0, height },
                            { { 24, 6, height + 8 }, { 2, 20, 31 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16217), { 0, 0, height },
                            { { 0, 6, height + 24 }, { 32, 20, 3 } });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16207), { 0, 0, height },
                            { { -8, 6, height }, { 2, 20, 31 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16210), { 0, 0, height },
                            { { 24, 6, height + 8 }, { 2, 20, 63 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16213), { 0, 0, height },
                            { { 24, 6, height + 8 }, { 2, 20, 63 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16216), { 0, 0, height },
                            { { -8, 6, height }, { 2, 20, 31 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16206), { 0, 0, height },
                            { { 4, 6, height + 8 }, { 2, 20, 31 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16209), { 0, 0, height },
                            { { 24, 6, height + 8 }, { 2, 20, 31 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16212), { 0, 0, height },
                            { { 24, 6, height + 8 }, { 2, 20, 31 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(16215), { 0, 0, height },
                            { { 4, 6, height + 8 }, { 2, 20, 31 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88);
                break;
            case 3:
                break;
        }
    }

    static void trackPaintMultiDimInvertedUp90ToFlatQuarterLoop(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26537), { 0, 0, height - 16 },
                            { { 28, 6, height - 8 }, { 2, 20, 31 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26540), { 0, 0, height - 16 },
                            { { 0, 6, height - 8 }, { 2, 20, 31 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26543), { 0, 0, height - 16 },
                            { { 0, 6, height - 8 }, { 2, 20, 31 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26546), { 0, 0, height - 16 },
                            { { 28, 6, height - 8 }, { 2, 20, 31 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26538), { 0, 0, height - 16 },
                            { { 24, 6, height - 16 }, { 2, 20, 31 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26541), { 0, 0, height - 16 },
                            { { 0, 6, height - 8 }, { 2, 20, 63 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26544), { 0, 0, height - 16 },
                            { { 0, 6, height - 8 }, { 2, 20, 63 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26547), { 0, 0, height - 16 },
                            { { 24, 6, height - 16 }, { 2, 20, 31 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26539), { 0, 0, height - 16 },
                            { { 0, 6, height - 16 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26542), { 0, 0, height - 16 },
                            { { 0, 6, height - 8 }, { 2, 20, 31 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26545), { 0, 0, height - 16 },
                            { { 0, 6, height - 8 }, { 2, 20, 31 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26548), { 0, 0, height - 16 },
                            { { 0, 6, height - 16 }, { 32, 20, 3 } });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
                break;
        }
    }

    static void trackPaintDiagBrakes(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        TrackPaintUtilDiagTilesPaint(
            session, -3, height + 24, direction, trackSequence, kMultidimDiagBrakeImages, defaultDiagTileOffsets,
            defaultDiagBoundLengths, nullptr);

        const uint16_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
        PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

        if (trackSequence == 3)
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, kDiagSupportPlacement[direction], 0, height + 36, session.SupportColours);
        }
    }

    static void trackPaintDiagBlockBrakes(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TrackElement& trackElement, const SupportType supportType)
    {
        TrackPaintUtilDiagTilesPaint(
            session, -3, height + 24, direction, trackSequence, kMultidimDiagBlockBrakeImages[trackElement.IsBrakeClosed()],
            defaultDiagTileOffsets, defaultDiagBoundLengths, nullptr);

        const uint16_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
        PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

        if (trackSequence == 3)
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, kDiagSupportPlacement[direction], 0, height + 36, session.SupportColours);
        }
    }

    constexpr std::array<std::remove_reference<TrackPaintFunction>::type*, EnumValue(TrackElemType::count)>
        kTrackPaintFunctions = []() {
            using enum TrackElemType;
            std::array<std::remove_reference<TrackPaintFunction>::type*, EnumValue(TrackElemType::count)> fns{};
            fns.fill(TrackPaintFunctionDummy);
            fns[EnumValue(flat)] = trackPaintFlat;
            fns[EnumValue(up25)] = trackPaintUp25;
            fns[EnumValue(up60)] = trackPaintUp60;
            fns[EnumValue(flatToUp25)] = trackPaintFlatToUp25;
            fns[EnumValue(up25ToUp60)] = trackPaintUp25ToUp60;
            fns[EnumValue(up60ToUp25)] = trackPaintUp60ToUp25;
            fns[EnumValue(up25ToFlat)] = trackPaintUp25ToFlat;
            fns[EnumValue(down25)] = trackPaintDown25;
            fns[EnumValue(down60)] = trackPaintDown60;
            fns[EnumValue(flatToDown25)] = trackPaintFlatToDown25;
            fns[EnumValue(down25ToDown60)] = trackPaintDown25ToDown60;
            fns[EnumValue(down60ToDown25)] = trackPaintDown60ToDown25;
            fns[EnumValue(down25ToFlat)] = trackPaintDown25ToFlat;
            fns[EnumValue(leftQuarterTurn5Tiles)] = trackPaintLeftQuarterTurn5Tiles;
            fns[EnumValue(rightQuarterTurn5Tiles)] = trackPaintRightQuarterTurn5Tiles;
            fns[EnumValue(flatToLeftBank)] = trackPaintFlatToLeftBank;
            fns[EnumValue(flatToRightBank)] = trackPaintFlatToRightBank;
            fns[EnumValue(leftBankToFlat)] = trackPaintLeftBankToFlat;
            fns[EnumValue(rightBankToFlat)] = trackPaintRightBankToFlat;
            fns[EnumValue(bankedLeftQuarterTurn5Tiles)] = trackPaintBankedLeftQuarterTurn5Tiles;
            fns[EnumValue(bankedRightQuarterTurn5Tiles)] = trackPaintBankedRightQuarterTurn5Tiles;
            fns[EnumValue(leftBankToUp25)] = trackPaintLeftBankToUp25;
            fns[EnumValue(rightBankToUp25)] = trackPaintRightBankToUp25;
            fns[EnumValue(up25ToLeftBank)] = trackPaintUp25ToLeftBank;
            fns[EnumValue(up25ToRightBank)] = trackPaintUp25ToRightBank;
            fns[EnumValue(leftBankToDown25)] = trackPaintLeftBankToDown25;
            fns[EnumValue(rightBankToDown25)] = trackPaintRightBankToDown25;
            fns[EnumValue(down25ToLeftBank)] = trackPaintDown25ToLeftBank;
            fns[EnumValue(down25ToRightBank)] = trackPaintDown25ToRightBank;
            fns[EnumValue(leftBank)] = trackPaintLeftBank;
            fns[EnumValue(rightBank)] = trackPaintRightBank;
            fns[EnumValue(sBendLeft)] = trackPaintSBendLeft;
            fns[EnumValue(sBendRight)] = trackPaintSBendRight;
            fns[EnumValue(leftQuarterTurn3Tiles)] = trackPaintLeftQuarterTurn3Tiles;
            fns[EnumValue(rightQuarterTurn3Tiles)] = trackPaintRightQuarterTurn3Tiles;
            fns[EnumValue(leftBankedQuarterTurn3Tiles)] = trackPaintLeftBankedQuarterTurn3Tiles;
            fns[EnumValue(rightBankedQuarterTurn3Tiles)] = trackPaintRightBankedQuarterTurn3Tiles;
            fns[EnumValue(brakes)] = trackPaintBrakes;
            fns[EnumValue(onRidePhoto)] = trackPaintOnRidePhoto;
            fns[EnumValue(up90)] = trackPaintUp90;
            fns[EnumValue(down90)] = trackPaintDown90;
            fns[EnumValue(up60ToUp90)] = trackPaintUp60ToUp90;
            fns[EnumValue(down90ToDown60)] = trackPaintDown90ToDown60;
            fns[EnumValue(up90ToUp60)] = trackPaintUp90ToUp60;
            fns[EnumValue(down60ToDown90)] = trackPaintDown60ToDown90;
            fns[EnumValue(leftEighthToDiag)] = trackPaintLeftEighthToDiag;
            fns[EnumValue(rightEighthToDiag)] = trackPaintRightEighthToDiag;
            fns[EnumValue(leftEighthToOrthogonal)] = trackPaintLeftEighthToOrthogonal;
            fns[EnumValue(rightEighthToOrthogonal)] = trackPaintRightEighthToOrthogonal;
            fns[EnumValue(leftEighthBankToDiag)] = trackPaintLeftEighthBankToDiag;
            fns[EnumValue(rightEighthBankToDiag)] = trackPaintRightEighthBankToDiag;
            fns[EnumValue(leftEighthBankToOrthogonal)] = trackPaintLeftEighthBankToOrthogonal;
            fns[EnumValue(rightEighthBankToOrthogonal)] = trackPaintRightEighthBankToOrthogonal;
            fns[EnumValue(diagFlat)] = trackPaintDiagFlat;
            fns[EnumValue(diagUp25)] = trackPaintDiagUp25;
            fns[EnumValue(diagUp60)] = trackPaintDiagUp60;
            fns[EnumValue(diagFlatToUp25)] = trackPaintDiagFlatToUp25;
            fns[EnumValue(diagUp25ToUp60)] = trackPaintDiagUp25ToUp60;
            fns[EnumValue(diagUp60ToUp25)] = trackPaintDiagUp60ToUp25;
            fns[EnumValue(diagUp25ToFlat)] = trackPaintDiagUp25ToFlat;
            fns[EnumValue(diagDown25)] = trackPaintDiagDown25;
            fns[EnumValue(diagDown60)] = trackPaintDiagDown60;
            fns[EnumValue(diagFlatToDown25)] = trackPaintDiagFlatToDown25;
            fns[EnumValue(diagDown25ToDown60)] = trackPaintDiagDown25ToDown60;
            fns[EnumValue(diagDown60ToDown25)] = trackPaintDiagDown60ToDown25;
            fns[EnumValue(diagDown25ToFlat)] = trackPaintDiagDown25ToFlat;
            fns[EnumValue(diagFlatToLeftBank)] = trackPaintDiagFlatToLeftBank;
            fns[EnumValue(diagFlatToRightBank)] = trackPaintDiagFlatToRightBank;
            fns[EnumValue(diagLeftBankToFlat)] = trackPaintDiagLeftBankToFlat;
            fns[EnumValue(diagRightBankToFlat)] = trackPaintDiagRightBankToFlat;
            fns[EnumValue(diagLeftBankToUp25)] = trackPaintDiagLeftBankToUp25;
            fns[EnumValue(diagRightBankToUp25)] = trackPaintDiagRightBankToUp25;
            fns[EnumValue(diagUp25ToLeftBank)] = trackPaintDiagUp25ToLeftBank;
            fns[EnumValue(diagUp25ToRightBank)] = trackPaintDiagUp25ToRightBank;
            fns[EnumValue(diagLeftBankToDown25)] = trackPaintDiagLeftBankToDown25;
            fns[EnumValue(diagRightBankToDown25)] = trackPaintDiagRightBankToDown25;
            fns[EnumValue(diagDown25ToLeftBank)] = trackPaintDiagDown25ToLeftBank;
            fns[EnumValue(diagDown25ToRightBank)] = trackPaintDiagDown25ToRightBank;
            fns[EnumValue(diagLeftBank)] = trackPaintDiagLeftBank;
            fns[EnumValue(diagRightBank)] = trackPaintDiagRightBank;
            fns[EnumValue(leftFlyerTwistDown)] = trackPaintLeftFlyerTwistDown;
            fns[EnumValue(rightFlyerTwistDown)] = trackPaintRightFlyerTwistDown;
            fns[EnumValue(multiDimInvertedFlatToDown90QuarterLoop)] = trackPaintMultiDimInvertedFlatToDown90QuarterLoop;
            fns[EnumValue(blockBrakes)] = trackPaintBlockBrakes;
            fns[EnumValue(multiDimInvertedUp90ToFlatQuarterLoop)] = trackPaintMultiDimInvertedUp90ToFlatQuarterLoop;
            fns[EnumValue(diagBrakes)] = trackPaintDiagBrakes;
            fns[EnumValue(diagBlockBrakes)] = trackPaintDiagBlockBrakes;
            return fns;
        }();
} // namespace OpenRCT2::MultiDimensionRCInverted

TrackPaintFunction getTrackPaintFunctionMultiDimensionRCInverted(const OpenRCT2::TrackElemType trackType)
{
    return *OpenRCT2::MultiDimensionRCInverted::kTrackPaintFunctions[EnumValue(trackType)];
}
