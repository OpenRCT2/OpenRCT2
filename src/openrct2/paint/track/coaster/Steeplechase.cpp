/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../../world/Map.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr MetalSupportType kSupportType = MetalSupportType::Stick;

static constexpr const uint32_t SteeplechaseRCDiagBrakeImages[kNumOrthogonalDirections] = {
    SPR_G2_STEEPLECHASE_DIAG_BRAKES,
    SPR_G2_STEEPLECHASE_DIAG_BRAKES + 1,
    SPR_G2_STEEPLECHASE_DIAG_BRAKES,
    SPR_G2_STEEPLECHASE_DIAG_BRAKES + 1,
};

/** rct2: 0x008A59A8 */
static void SteeplechaseTrackFlat(
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
                    session, direction, session.TrackColours.WithIndex(28635), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28636), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
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
                    session, direction, session.TrackColours.WithIndex(28633), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28634), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                break;
        }
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void SteeplechaseTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr uint32_t imageIds[4][3] = {
        { 28635, SPR_STATION_BASE_B_SW_NE },
        { 28636, SPR_STATION_BASE_B_NW_SE },
        { 28635, SPR_STATION_BASE_B_SW_NE },
        { 28636, SPR_STATION_BASE_B_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][1]),
        { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 3 } });
    PaintAddImageAsChildRotated(
        session, direction, session.TrackColours.WithIndex(imageIds[direction][0]), { 0, 6, height },
        { { 0, 0, height }, { 32, 20, 3 } });
    DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Boxed);
    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TunnelType::SquareFlat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A59B8 */
static void SteeplechaseTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28649), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 8, height, session.SupportColours);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28655), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 8, height, session.SupportColours);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28651), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 8, height, session.SupportColours);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28656), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 8, height, session.SupportColours);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28637), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 8, height, session.SupportColours);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28643), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 8, height, session.SupportColours);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28639), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 8, height, session.SupportColours);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28644), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 8, height, session.SupportColours);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelType::StandardSlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelType::StandardSlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A59C8 */
static void SteeplechaseTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28650), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 3, height, session.SupportColours);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28657), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 2, height, session.SupportColours);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28652), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 2, height, session.SupportColours);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28658), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 2, height, session.SupportColours);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28638), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 3, height, session.SupportColours);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28645), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 2, height, session.SupportColours);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28640), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 2, height, session.SupportColours);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28646), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 2, height, session.SupportColours);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardSlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008A59D8 */
static void SteeplechaseTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28654), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 6, height, session.SupportColours);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28660), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 6, height, session.SupportColours);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28653), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 5, height, session.SupportColours);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28659), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 5, height, session.SupportColours);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28642), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 6, height, session.SupportColours);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28648), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 6, height, session.SupportColours);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28641), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 5, height, session.SupportColours);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28647), { 0, 6, height }, { 32, 20, 3 });
                MetalASupportsPaintSetup(
                    session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 5, height, session.SupportColours);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelType::StandardFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelType::StandardFlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008A59E8 */
static void SteeplechaseTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    SteeplechaseTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A59F8 */
static void SteeplechaseTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    SteeplechaseTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A5A08 */
static void SteeplechaseTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    SteeplechaseTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A5A18 */
static void SteeplechaseTrackLeftQuarterTurn5(
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
                        session, direction, session.TrackColours.WithIndex(28698), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 1, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28703), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 1, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28708), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height - 3, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28693), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height - 1, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
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
                        session, direction, session.TrackColours.WithIndex(28697), { 0, 0, height }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28702), { 0, 0, height }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28707), { 0, 16, height }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28692), { 0, 16, height }, { 32, 16, 3 });
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28696), { 0, 16, height }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28701), { 16, 16, height }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28706), { 16, 0, height }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28691), { 0, 0, height }, { 16, 16, 3 });
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
                        session, direction, session.TrackColours.WithIndex(28695), { 16, 0, height }, { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28700), { 0, 0, height }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28705), { 0, 0, height }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28690), { 16, 0, height }, { 16, 32, 3 });
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28694), { 6, 0, height }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height - 3, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28699), { 6, 0, height }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28704), { 6, 0, height }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28689), { 6, 0, height }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height - 2, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelType::StandardFlat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelType::StandardFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A5A28 */
static void SteeplechaseTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    SteeplechaseTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A5A38 */
static void SteeplechaseTrackSBendLeft(
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
                        session, direction, session.TrackColours.WithIndex(28665), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28672), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28668), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28669), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height - 2, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28666), { 0, 0, height }, { 33, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::TopLeftSide, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28671), { 0, 0, height }, { 33, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::TopRightSide, 0, height,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28667), { 0, 6, height }, { 33, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28670), { 0, 6, height }, { 33, 26, 3 });
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28667), { 0, 6, height }, { 33, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28670), { 0, 6, height }, { 33, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28666), { 0, 0, height }, { 33, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::TopLeftSide, 0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28671), { 0, 0, height }, { 33, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::TopRightSide, 0, height,
                        session.SupportColours);
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28668), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28669), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height - 2, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28665), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28672), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelType::StandardFlat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelType::StandardFlat);
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A5A48 */
static void SteeplechaseTrackSBendRight(
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
                        session, direction, session.TrackColours.WithIndex(28661), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28676), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28664), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28673), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28662), { 0, 6, height }, { 33, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::BottomRightSide, 0, height - 2, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28675), { 0, 6, height }, { 33, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::BottomLeftSide, 0, height,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28663), { 0, 0, height }, { 33, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28674), { 0, 0, height }, { 33, 26, 3 });
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28663), { 0, 0, height }, { 33, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28674), { 0, 0, height }, { 33, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28662), { 0, 6, height }, { 33, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::BottomRightSide, 0, height - 2, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28675), { 0, 6, height }, { 33, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::BottomLeftSide, 0, height,
                        session.SupportColours);
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28664), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28673), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28661), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28676), { 0, 6, height }, { 33, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelType::StandardFlat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelType::StandardFlat);
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A5A88 */
static void SteeplechaseTrackLeftQuarterTurn3(
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
                        session, direction, session.TrackColours.WithIndex(28682), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28685), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 1, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28688), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28679), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height - 3, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
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
                        session, direction, session.TrackColours.WithIndex(28681), { 16, 0, height }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28684), { 0, 0, height }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28687), { 0, 16, height }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28678), { 16, 16, height }, { 16, 16, 3 });
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28680), { 6, 0, height }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28683), { 6, 0, height }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 1, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28686), { 6, 0, height }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 3, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28677), { 6, 0, height }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height - 3, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelType::StandardFlat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelType::StandardFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A5A98 */
static void SteeplechaseTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    SteeplechaseTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A5AA8 */
static void SteeplechaseTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(28635), { 0, 6, height }, { 32, 20, 3 });
            MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(28636), { 0, 6, height }, { 32, 20, 3 });
            MetalASupportsPaintSetup(
                session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A5AD8 */
static void SteeplechaseTrackLeftEighthToDiag(
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
                        session, direction, session.TrackColours.WithIndex(28725), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28729), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28733), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28737), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28726), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28730), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28734), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28738), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28727), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28731), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28735), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28739), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28728), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::BottomCorner, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28732), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::LeftCorner, 0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28736), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::TopCorner, 0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28740), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::RightCorner, 0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A5AE8 */
static void SteeplechaseTrackRightEighthToDiag(
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
                        session, direction, session.TrackColours.WithIndex(28709), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28713), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28717), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28721), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28710), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28714), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28718), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28722), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28711), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28715), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28719), { 0, 0, height },
                        { { 4, 4, height }, { 28, 28, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28723), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28712), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::LeftCorner, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28716), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28720), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 3 } });
                    MetalASupportsPaintSetup(
                        session, kSupportType, MetalSupportPlace::RightCorner, 0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28724), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 0, height,
                        session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A5AF8 */
static void SteeplechaseTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    SteeplechaseTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A5B08 */
static void SteeplechaseTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    SteeplechaseTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008A5AC8 */
static void SteeplechaseTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    constexpr ImageIndex images[2][kNumOrthogonalDirections] = {
        { 28741, 28742, 28743, 28744 },
        { 28757, 28758, 28759, 28760 },
    };
    TrackPaintUtilDiagTilesPaintExtra(
        session, 3, height, direction, trackSequence, images[trackElement.HasChain()], kSupportType);
}

static void SteeplechaseTrackDiagBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    TrackPaintUtilDiagTilesPaint(
        session, 3, height, direction, trackSequence, SteeplechaseRCDiagBrakeImages, defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    if (trackSequence == 3)
    {
        MetalASupportsPaintSetup(session, kSupportType, kDiagSupportPlacement[direction], 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A5B38 */
static void SteeplechaseTrackDiag25DegUp(
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
                            session, direction, session.TrackColours.WithIndex(28772), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28756), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28769), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28753), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28771), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28755), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 6, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28770), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 6, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 6, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 6, height,
                            session.SupportColours);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 6, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28754), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 6, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 6, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 6, height,
                            session.SupportColours);
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x008A5B18 */
static void SteeplechaseTrackDiagFlatTo25DegUp(
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
                            session, direction, session.TrackColours.WithIndex(28764), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28748), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28761), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28745), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28763), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28747), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 0, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28762), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 0, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 0, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 0, height,
                            session.SupportColours);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 0, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28746), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 0, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 0, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 0, height,
                            session.SupportColours);
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
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008A5B28 */
static void SteeplechaseTrackDiag25DegUpToFlat(
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
                            session, direction, session.TrackColours.WithIndex(28768), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28752), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28765), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28749), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28767), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28751), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 4, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28766), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 4, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 4, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 4, height,
                            session.SupportColours);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 4, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28750), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 4, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 4, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 4, height,
                            session.SupportColours);
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x008A5B68 */
static void SteeplechaseTrackDiag25DegDown(
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
                            session, direction, session.TrackColours.WithIndex(28770), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28754), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28771), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28755), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28769), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28753), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 6, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28772), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 6, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 6, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 6, height,
                            session.SupportColours);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 6, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28756), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 6, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 6, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 6, height,
                            session.SupportColours);
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x008A5B48 */
static void SteeplechaseTrackDiagFlatTo25DegDown(
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
                            session, direction, session.TrackColours.WithIndex(28766), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28750), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(28767), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28751), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(28765), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28749), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 4, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28768), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 4, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 4, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 4, height,
                            session.SupportColours);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 4, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28752), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 4, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 4, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 4, height,
                            session.SupportColours);
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
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A5B58 */
static void SteeplechaseTrackDiag25DegDownToFlat(
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
                            session, direction, session.TrackColours.WithIndex(28762), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28746), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28763), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28747), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28761), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28745), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 0, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28764), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 0, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 0, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 0, height,
                            session.SupportColours);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::LeftCorner, 0, height, session.SupportColours);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28748), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::TopCorner, 0, height,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, kSupportType, MetalSupportPlace::RightCorner, 0, height, session.SupportColours);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportType::StickAlt, MetalSupportPlace::BottomCorner, 0, height,
                            session.SupportColours);
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
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008A5AB8 */
static void SteeplechaseTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(28635), { 0, 6, height }, { 32, 20, 3 });
            MetalASupportsPaintSetup(session, kSupportType, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(28636), { 0, 6, height }, { 32, 20, 3 });
            MetalASupportsPaintSetup(
                session, MetalSupportType::StickAlt, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionSteeplechase(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return SteeplechaseTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return SteeplechaseTrackStation;
        case TrackElemType::Up25:
            return SteeplechaseTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return SteeplechaseTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return SteeplechaseTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return SteeplechaseTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return SteeplechaseTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return SteeplechaseTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return SteeplechaseTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return SteeplechaseTrackRightQuarterTurn5;
        case TrackElemType::SBendLeft:
            return SteeplechaseTrackSBendLeft;
        case TrackElemType::SBendRight:
            return SteeplechaseTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return SteeplechaseTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return SteeplechaseTrackRightQuarterTurn3;
        case TrackElemType::Brakes:
            return SteeplechaseTrackBrakes;
        case TrackElemType::LeftEighthToDiag:
            return SteeplechaseTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return SteeplechaseTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return SteeplechaseTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return SteeplechaseTrackRightEighthToOrthogonal;
        case TrackElemType::DiagFlat:
            return SteeplechaseTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return SteeplechaseTrackDiag25DegUp;
        case TrackElemType::DiagFlatToUp25:
            return SteeplechaseTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return SteeplechaseTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return SteeplechaseTrackDiag25DegDown;
        case TrackElemType::DiagFlatToDown25:
            return SteeplechaseTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return SteeplechaseTrackDiag25DegDownToFlat;
        case TrackElemType::BlockBrakes:
            return SteeplechaseTrackBlockBrakes;
        case TrackElemType::DiagBrakes:
        case TrackElemType::DiagBlockBrakes:
            return SteeplechaseTrackDiagBrakes;
    }
    return nullptr;
}
